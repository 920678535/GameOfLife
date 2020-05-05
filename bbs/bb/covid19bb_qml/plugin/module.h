#pragma once

#include <net/phoneyou/covid19/bb/Messages.pb.h>
#include <net/phoneyou/covid19/bb/Descriptors.pb.h>
#include <net/phoneyou/covid19/bb/SingletonDescriptors.pb.h>
#include <net/phoneyou/covid19/bb/Entities.pb.h>

#include <g3logwrapper/log.h>
#include <gbb/gbb_constants.h>
#include <gbbinfra/descriptorregister.h>
#include <gbbinfra/descriptortablestorage.h>
#include <gbbinfra/entityregister.h>
#include <gbbinfra/entitytablestorage.h>

#include <amsterqt/bbaccessor/bbaccessor.h>

using namespace amster;
using namespace amster::gbbinfra;
using namespace net::phoneyou::covid19::bb;

template <class T>
T* gbbupper_cast(amster::gbbinfra::EntityBase* _entity)
{
    auto& _options = T::descriptor()->options();
    CHECKF(_options.HasExtension(amster::gbbinfra::entity_type),
        "the gbb entity [{}] has no type field !",
        T::descriptor()->name());

    gbbinfra::EntityType _type = static_cast<gbbinfra::EntityType>(_options.GetExtension(amster::gbbinfra::entity_type));
    auto _super = EntityRegister::superEntity(_entity);
    while (_super && _type != gbbinfra::EntityRegister::truthType(_super)) {
        _super = EntityRegister::superEntity(_super);
    }
    LOGF_IF(WARNING,
        !_super,
        "{} is not a super entity type of [{}]",
        T::descriptor()->full_name(),
        _entity->GetDescriptor()->full_name());
    return dynamic_cast<T*>(_super);
}

template <class T>
bool gbbupper_judge(amster::gbbinfra::EntityBase* _entity)
{
    auto& _options = T::descriptor()->options();
    CHECKF(_options.HasExtension(amster::gbbinfra::entity_type),
        "the gbb entity [{}] has no type field !",
        T::descriptor()->name());

    gbbinfra::EntityType _type = static_cast<gbbinfra::EntityType>(_options.GetExtension(amster::gbbinfra::entity_type));
    auto _super = EntityRegister::superEntity(_entity);
    while (_super && _type != EntityRegister::truthType(_super)) {
        _super = EntityRegister::superEntity(_super);
    }
    return _super != Q_NULLPTR;
}
// register rpc service to qml in this bb
#define FOR_EACH_NET_PHONEYOU_COVID19_BB_MESSAGE_RPC_REGISTER_TEMPLATE(F) \
    F("scenario_init", net::phoneyou::covid19::bb::message::ScenarioInit, scenario_init) \
    F("change_cell_status", net::phoneyou::covid19::bb::message::ChangeCellStaus, change_cell_status) \
    F("update_lemology_model", net::phoneyou::covid19::bb::message::UpdateLemologyModel, update_lemology_model) \
    F("reset_all_cells", net::phoneyou::covid19::bb::message::ResetAllCells, reset_all_cells) \
    F("generate_a_city", net::phoneyou::covid19::bb::message::GenerateACity, generate_a_city) \

#define REGISTER_TYPE_WITH_NAME(T, NAME) \
    qmlRegisterUncreatableType<T>(uri, versionMajor, versionMinor, #NAME, QString::fromLatin1("descriptor cannot be created by qml."));

// EntityCast
#define DEFINE_ENTITY_PROPERTY(T, NAME) \
    Q_PROPERTY(T* to##NAME READ to##NAME NOTIFY attachedEntityChanged)

#define DEFINE_ENTITY_CAST(T, NAME)                                \
    T* to##NAME()                                                  \
    {                                                              \
        if (!attachedEntity_) {                                    \
            return 0;                                              \
        }                                                          \
        auto* _res = qobject_cast<T*>(attachedEntity_);            \
        if (!_res) {                                               \
            if (auto _message                                      \
                = dynamic_cast<EntityBase*>(attachedEntity_)) {    \
                _res = gbbupper_cast<T>(_message); \
            }                                                      \
        }                                                          \
        return _res;                                               \
    }

//EntityCastSingleton
#define DEFINE_ENTITY_CAST_SINGLETON(T, NAME)                      \
    Q_INVOKABLE T* to##NAME(QObject* _entity)                      \
    {                                                              \
        T* _res = qobject_cast<T*>(_entity);                       \
        if (!_res) {                                               \
            if (auto _message                                      \
                = dynamic_cast<EntityBase*>(_entity)) {            \
                _res = gbbupper_cast<T>(_message); \
            }                                                      \
        }                                                          \
        return _res;                                               \
    }
#define DEFINE_JUDGE_ENTITY_CAST_SINGLETON(T, NAME)                 \
    Q_INVOKABLE bool canConvert##NAME(QObject* _entity)             \
    {                                                               \
        T* _res = qobject_cast<T*>(_entity);                        \
        if (!_res) {                                                \
            if (auto _message                                       \
                = dynamic_cast<EntityBase*>(_entity)) {             \
                return gbbupper_judge<T>(_message); \
            }                                                       \
            return false;                                           \
        }                                                           \
        return true;                                                \
    }

#define DEFINE_ENUM_NAME_SINGLETON(T, NAME)                                 \
    Q_INVOKABLE QString get##NAME##Name(int value)                          \
    {                                                                       \
        if (T##_IsValid(value)) {                                           \
            return QString::fromStdString(T##_Name(static_cast<T>(value))); \
        }                                                                   \
        return QString();                                                   \
    }

//Notification
#define DEFINE_PUBLISH_MESSAGE(T, NAME)                                                           \
    Q_INVOKABLE void pub_##NAME(QJSValue _data, bool _blocking = false)                           \
    {                                                                                             \
        auto _name = QString(QLatin1String(#T)).replace(QLatin1String("::"), QLatin1String(".")); \
        publishMessage(_name, _data, _blocking);                                                  \
    }
#define DEFINE_SUBSCRIBE_MESSAGE(T, NAME)                                                         \
    Q_INVOKABLE void sub_##NAME(QJSValue _data)                                                   \
    {                                                                                             \
        auto _name = QString(QLatin1String(#T)).replace(QLatin1String("::"), QLatin1String(".")); \
        subscribeMessage(_name, _data);                                                           \
    }

#define DEFINE_LISTEN_MESSAGE(T, NAME)                                                            \
    Q_INVOKABLE void listen_##NAME(QJSValue _data)                                                \
    {                                                                                             \
        auto _name = QString(QLatin1String(#T)).replace(QLatin1String("::"), QLatin1String(".")); \
                                                                                                  \
        listenMessage(_name, _data);                                                              \
    }

#define DEFINE_RPC_MESSAGE(RPCNAME, T, NAME)                                                                                                                                \
    Q_INVOKABLE void rpc_##NAME(QJSValue _msgData, QJSValue _responseCallback = QJSValue(), QJSValue _failedCallback = QJSValue(), int _interval = 1000, bool _sync = true) \
    {                                                                                                                                                                       \
        auto _name = QString(QLatin1String(#T)).replace(QLatin1String("::"), QLatin1String("."));                                                                           \
        doRPC(QLatin1String(RPCNAME), _name, _msgData, _responseCallback, _failedCallback, _interval, _sync);                                                               \
    }

//singleton descriptor
#define DEFINE_SINGLETON_DESCRIPTOR_PROPERTY(T, NAME) \
    Q_PROPERTY(T* NAME READ NAME NOTIFY attached##NAME##Changed)

#define DEFINE_SINGLETON_DESCRIPTOR(T, NAME)                                                                                          \
    T* NAME()                                                                                                                         \
    {                                                                                                                                 \
        auto& _options = T::descriptor()->options();                                                                                  \
        CHECKF(_options.HasExtension(amster::gbbinfra::descriptor_type) && _options.HasExtension(amster::gbbinfra::singleton),        \
            "[{}] is not singleton descriptor !",                                                                                     \
            T::descriptor()->name());                                                                                                 \
        gbbinfra::DescriptorType _type = static_cast<gbbinfra::EntityType>(_options.GetExtension(amster::gbbinfra::descriptor_type)); \
                                                                                                                                      \
        return dynamic_cast<T*>(singletonDescriptor(_type));                                                                          \
    }

#define DEFINE_SINGLETON_DESCRIPTOR_SIGNAL(T, NAME) \
    void attached##NAME##Changed();

#define DEFINE_EMIT_SINGLETON_DESCRIPTOR_SIGNAL(T, NAME) \
    emit attached##NAME##Changed();

class EntityCast : public QObject {
    Q_OBJECT
    // generated cast property
    Q_PROPERTY(QObject* from READ attachedEntity WRITE setAttachedEntity NOTIFY attachedEntityChanged)
    FOR_EACH_NET_PHONEYOU_COVID19_BB_ENTITIES_MESSAGE_TEMPLATE(DEFINE_ENTITY_PROPERTY)

public:
    FOR_EACH_NET_PHONEYOU_COVID19_BB_ENTITIES_MESSAGE_TEMPLATE(DEFINE_ENTITY_CAST)

public:
    EntityCast(QObject* parent = Q_NULLPTR);

    QObject* attachedEntity() const;

public slots:
    void setAttachedEntity(QObject* attachedEntity);

signals:
    void attachedEntityChanged(QObject* attachedEntity);

private:
    QObject* attachedEntity_;
};

class EntityCastSingleton : public QObject {
    Q_OBJECT
public:

    FOR_EACH_NET_PHONEYOU_COVID19_BB_ENTITIES_MESSAGE_TEMPLATE(DEFINE_ENTITY_CAST_SINGLETON)
    FOR_EACH_NET_PHONEYOU_COVID19_BB_ENTITIES_MESSAGE_TEMPLATE(DEFINE_JUDGE_ENTITY_CAST_SINGLETON)
    FOR_EACH_NET_PHONEYOU_COVID19_BB_COMMON_ENUM_TEMPLATE(DEFINE_ENUM_NAME_SINGLETON)
};

class NotificationSingleton : public QObject {
    Q_OBJECT
public:
    NotificationSingleton(amster::qt::BBAccessor* _publisher, QJSEngine* _engine);
    virtual ~NotificationSingleton();

public:
    // 发送message
    FOR_EACH_NET_PHONEYOU_COVID19_BB_MESSAGES_MESSAGE_TEMPLATE(DEFINE_PUBLISH_MESSAGE)
    FOR_EACH_NET_PHONEYOU_COVID19_BB_MESSAGES_MESSAGE_TEMPLATE(DEFINE_SUBSCRIBE_MESSAGE)

    FOR_EACH_NET_PHONEYOU_COVID19_BB_MESSAGES_MESSAGE_TEMPLATE(DEFINE_LISTEN_MESSAGE)

    FOR_EACH_NET_PHONEYOU_COVID19_BB_MESSAGE_RPC_REGISTER_TEMPLATE(DEFINE_RPC_MESSAGE)

    // 将descriptor作为message发送
    Q_INVOKABLE void pubDescriptorAsMessage(
        quint64 _type,
        QJSValue _data,
        bool _blocking = false);

    // 一个实体的特定descriptor作为消息发送（不建议使用）
    Q_INVOKABLE void pubEntityDescriptorAsMessage(
        quint64 _metId,
        quint64 _type,
        QJSValue _data,
        bool _isPatch = false,
        bool _blocking = false);

private:
    void publishMessage(
        const QString& _name,
        const QJSValue& _data,
        bool _blocking = false) const;
    void publishEntityDescriptorAsMessage(
        quint64 _metId,
        const QString& _name,
        const QJSValue& _data,
        bool _isPatch = false,
        bool _blocking = false) const;

    void subscribeMessage(const QString& _publisherName, QJSValue _callback);
    QString listenMessage(const QString& _listenerName, QJSValue _callback);
    void doRPC(
        const QString& _rpcName,
        const QString& _reqMsgName,
        const QJSValue& _reqMsgData,
        QJSValue _responseCallback,
        QJSValue _failedCallback,
        int _interval,
        bool _sync = true) const;

    QString getDescriptorName(quint64 _type) const;
    const QByteArray formatMessageData(const QJSValue& _data) const;

private:
    amster::qt::BBAccessor* gbbAccessor_;
    // We do _NOT_ own the original `QJSEngine` object, so if it need to be
    // managed by `shared_ptr`, an empty deleter should be provided.
    // After shutdown initiated, `this` will be destroyed eventually, so as
    // the `QJSEngine` shared_ptr object. Because the closure of notification
    // callbacks only hold the `weak_ptr` object, which can't stop it been
    // destroyed. After destroyed, any pending or incoming callbacks will
    // be ware of the destroy by checking the result of `lock()` of the
    // `weak_ptr` and do nothing wrong.
    using QJSEnginePtr = std::shared_ptr<QJSEngine>;
    using QJSEngineWPtr = std::weak_ptr<QJSEngine>;
    QJSEnginePtr engine_;
};

//单例描述子导出
class GlobalSingleton : public QObject {
    Q_OBJECT
    FOR_EACH_NET_PHONEYOU_COVID19_BB_SINGLETONDESCRIPTORS_MESSAGE_TEMPLATE(DEFINE_SINGLETON_DESCRIPTOR_PROPERTY)
public:
    GlobalSingleton(amster::qt::BBAccessor* gbbAccessor_);

public:
    FOR_EACH_NET_PHONEYOU_COVID19_BB_SINGLETONDESCRIPTORS_MESSAGE_TEMPLATE(DEFINE_SINGLETON_DESCRIPTOR)

signals:
    FOR_EACH_NET_PHONEYOU_COVID19_BB_SINGLETONDESCRIPTORS_MESSAGE_TEMPLATE(DEFINE_SINGLETON_DESCRIPTOR_SIGNAL)

private slots:
    void initSingletonDescriptorSignal()
    {
        FOR_EACH_NET_PHONEYOU_COVID19_BB_SINGLETONDESCRIPTORS_MESSAGE_TEMPLATE(DEFINE_EMIT_SINGLETON_DESCRIPTOR_SIGNAL)
    }

private:
    amster::gbbinfra::DescriptorBase* singletonDescriptor(amster::gbbinfra::DescriptorType _type) const;

private:
    amster::qt::BBAccessor* gbbAccessor_;
};
