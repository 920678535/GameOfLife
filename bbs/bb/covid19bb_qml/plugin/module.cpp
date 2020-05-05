/****************************************************************************
**
** Copyright© 2014-2015 WUHAN PHONEYOU INFORMATICS CO,.LTD.
** Web-site: http://www.phoneyou.net/
** E-Mail:phoneyou@phoneyou.net
** This file is part of Roshan Framework.
**
****************************************************************************/
#include "module.h"
#include <QCMainThreadRunner>
#include <QJsonDocument>

#include <gbbinfra/descriptorregister.h>

//    using MessagePtr = amster::notification::MessagePtr;
//    using MessageConstPtr = amster::notification::MessageConstPtr;
//    using MessageBase = amster::notification::MessageBase;

template <typename T>
class MainThreadDeleter {
public:
    void operator()(T* _obj) const
    {
        MAIN_THREAD
        {
            delete _obj;
        };
    }
};

template <typename T, typename... Args>
inline std::shared_ptr<T> make_shared_with_main_thread_deleter(Args&&... args)
{
    return std::shared_ptr<T>(new T(std::forward<Args>(args)...), MainThreadDeleter<T>());
}

NotificationSingleton::NotificationSingleton(amster::qt::BBAccessor* _publisher,
    QJSEngine* _engine)
    : gbbAccessor_(_publisher)
    , engine_(_engine, [](decltype(_engine)) {})
{
}

NotificationSingleton::~NotificationSingleton()
{
    // Make sure nobody else accidentally hold a strong reference to it.
    // Several `QJSEngineWPtr`s of notification rely on this to work properly.
    CHECK(engine_.use_count() <= 1);
}

void NotificationSingleton::pubDescriptorAsMessage(
    quint64 _type,
    QJSValue _data,
    bool _blocking)
{
    auto _descriptorName = getDescriptorName(_type);
    publishMessage(_descriptorName, _data, _blocking);
}

void NotificationSingleton::pubEntityDescriptorAsMessage(
    quint64 _metId,
    quint64 _type,
    QJSValue _data,
    bool _isPatch,
    bool _blocking)
{
    publishEntityDescriptorAsMessage(
        _metId,
        getDescriptorName(_type),
        _data,
        _isPatch,
        _blocking);
}

QString NotificationSingleton::getDescriptorName(quint64 _type) const
{
    std::string _name;
    std::tie(_name, std::ignore) = DescriptorRegister::type2Name(_type);
    LOGF_IF(WARNING, _name.empty(), "can't find the descriptor type in descriptor table.[{}].", _type);
    return QString::fromStdString(_name);
}

void NotificationSingleton::publishMessage(
    const QString& _name,
    const QJSValue& _data,
    bool _blocking) const
{
    if (!gbbAccessor_ || !_data.isObject())
        return;
    QByteArray _msgData = formatMessageData(_data);
    return gbbAccessor_->publishMessage(_name, _msgData, _blocking);
}

void NotificationSingleton::publishEntityDescriptorAsMessage(
    quint64 _metId,
    const QString& _name,
    const QJSValue& _data,
    bool _isPatch,
    bool _blocking) const
{
    if (!gbbAccessor_ || !_data.isObject())
        return;
    return gbbAccessor_->publishEntityDescriptorAsMessage(
        _metId,
        _name,
        formatMessageData(_data),
        _isPatch,
        _blocking);
}

const QByteArray NotificationSingleton::formatMessageData(const QJSValue& _data) const
{
    QJsonDocument _doc = QJsonDocument::fromVariant(_data.toVariant());
    if (_doc.isNull()) {
        return QByteArray();
    }
    return _doc.toJson(QJsonDocument::Compact);
}

void NotificationSingleton::doRPC(
    const QString& _rpcName,
    const QString& _reqMsgName,
    const QJSValue& _reqMsgData,
    QJSValue _responseCallback,
    QJSValue _failedCallback,
    int _interval,
    bool _sync) const
{
    if (!gbbAccessor_ || !_reqMsgData.isObject())
        return;
    QByteArray _msgData = formatMessageData(_reqMsgData);
    auto _failedCallbackPtr = make_shared_with_main_thread_deleter<QJSValue>(_failedCallback);
    auto _responseCallbackPtr = make_shared_with_main_thread_deleter<QJSValue>(_responseCallback);
    auto _wpEngine = QJSEngineWPtr(engine_);
    auto _wrapper = [_failedCallbackPtr,
        _responseCallbackPtr,
        _wpEngine,
        _rpcName,
        _interval](
        MessageConstPtr _msg,
        gateway::RPCResultState _state) {
        MAIN_THREAD
        {
            auto _pEngine = _wpEngine.lock();
            if (nullptr == _pEngine)
                return;
            if (_state != gateway::RPCResultState::Success) {
                if (_failedCallbackPtr->isCallable()) {
                    auto _result = _failedCallbackPtr->call();
                    LOGF_IF(
                        WARNING,
                        _result.isError(),
                        "execute rpc {} timeout function failed: {}",
                        _rpcName.toStdString(),
                        _result.toString().toStdString());
                }
            } else {
                if (_responseCallbackPtr->isCallable()) {
                    std::remove_const_t<std::remove_reference_t<decltype(_msg)>::element_type>* _msgCopy = nullptr;
                    if (nullptr != _msg) {
                        _msgCopy = _msg->New();
                        CHECK(_msgCopy);
                        _msgCopy->CopyFrom(*_msg);
                    }

                    QJSValueList _args;
                    _args.append(_pEngine->newQObject(_msgCopy));
                    _msgCopy = nullptr;
                    auto _result = _responseCallbackPtr->call(_args);
                    LOGF_IF(WARNING,
                        _result.isError(),
                        "execute rpc {} result function failed: {}",
                        _rpcName.toStdString(),
                        _result.toString().toStdString());
                }
            }
        };
    };
    return gbbAccessor_->doRPC(
        _rpcName,
        _reqMsgName,
        _msgData,
        _wrapper,
        _interval,
        _sync);
}

void NotificationSingleton::subscribeMessage(const QString& _publisherName, QJSValue _callback)
{
    if (!gbbAccessor_ || !_callback.isCallable())
        return;
    auto _pCallback = make_shared_with_main_thread_deleter<QJSValue>(_callback);
    auto _wpEngine = QJSEngineWPtr(engine_);
    auto _wrapper = [_pCallback, _wpEngine](MessageConstPtr _msg) {
        MAIN_THREAD
        {
            auto _pEngine = _wpEngine.lock();
            if (nullptr == _pEngine)
                return;

            std::remove_const_t<std::remove_reference_t<decltype(_msg)>::element_type>* _msgCopy = nullptr;
            if (nullptr != _msg) {
                _msgCopy = _msg->New();
                CHECK(_msgCopy);
                _msgCopy->CopyFrom(*_msg);
            }

            QJSValueList _args;
            _args.append(_pEngine->newQObject(_msgCopy));
            _msgCopy = nullptr;
            _pCallback->call(_args);
        };
    };
    return gbbAccessor_->subscribeMessage(_publisherName, _wrapper);
}

QString NotificationSingleton::listenMessage(const QString& _listenerName, QJSValue _callback)
{
    if (!gbbAccessor_ || !_callback.isCallable())
        return QLatin1String("");
    auto _pCallback = make_shared_with_main_thread_deleter<QJSValue>(_callback);
    auto _wpEngine = QJSEngineWPtr(engine_);
    auto _wrapper = [_pCallback, _wpEngine](MessageConstPtr _msg) {
        MAIN_THREAD
        {
            auto _pEngine = _wpEngine.lock();
            if (nullptr == _pEngine)
                return;

            std::remove_const_t<std::remove_reference_t<decltype(_msg)>::element_type>* _msgCopy = nullptr;
            if (nullptr != _msg) {
                _msgCopy = _msg->New();
                CHECK(_msgCopy);
                _msgCopy->CopyFrom(*_msg);
            }

            QJSValueList _args;
            _args.append(_pEngine->newQObject(_msgCopy));
            _msgCopy = nullptr;
            _pCallback->call(_args);
        };
    };
    return gbbAccessor_->registerListener(_listenerName, _wrapper);
}

EntityCast::EntityCast(QObject* parent)
    : QObject(parent)
    , attachedEntity_(Q_NULLPTR)
{
}

QObject* EntityCast::attachedEntity() const
{
    return attachedEntity_;
}

void EntityCast::setAttachedEntity(QObject* attachedEntity)
{
    if (attachedEntity_ == attachedEntity)
        return;

    attachedEntity_ = attachedEntity;
    emit attachedEntityChanged(attachedEntity_);
}

amster::gbbinfra::DescriptorBase* GlobalSingleton::singletonDescriptor(amster::gbbinfra::DescriptorType _type) const
{
    if (!gbbAccessor_)
        return nullptr;
    return const_cast<amster::gbbinfra::DescriptorBase*>(gbbAccessor_->singletonDescriptor(_type));
}

GlobalSingleton::GlobalSingleton(amster::qt::BBAccessor* accessor_)
    : gbbAccessor_(accessor_)
{
    //        connect(accessor_, &importer::ASAccessor::connectedChanged, this, &GlobalSingleton::initSingletonDescriptorSignal);
}
