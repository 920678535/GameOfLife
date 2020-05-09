#include <QtQml>

#include <amsterqt/bbaccessor/bbaccessor.h>

#include <google/protobuf/wrappers.pb.h>

#include "module.h"

template <class T>
static QObject* entity_cast_provider(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new T();
}

template <class T>
static QObject* accessor_singleton_provider(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new T(amster::qt::BBAccessor::instance());
}

template <class T>
static QObject* accessor_notification_provider(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)

    return new T(amster::qt::BBAccessor::instance(), scriptEngine);
}

class MyPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char* uri)
    {
        Q_ASSERT(QLatin1String(uri) == QStringLiteral("net.phoneyou.LULU.BB"));

        int versionMajor = 1;
        int versionMinor = 0;

        // register types
        FOR_EACH_NET_PHONEYOU_LULU_BB_MESSAGES_MESSAGE_TEMPLATE(REGISTER_TYPE_WITH_NAME)
        qmlRegisterSingletonType<NotificationSingleton>(uri, versionMajor, versionMinor, "Notification", accessor_notification_provider<NotificationSingleton>);

        FOR_EACH_NET_PHONEYOU_LULU_BB_COMMON_MESSAGE_TEMPLATE(REGISTER_TYPE_WITH_NAME)
        FOR_EACH_NET_PHONEYOU_LULU_BB_DESCRIPTORS_MESSAGE_TEMPLATE(REGISTER_TYPE_WITH_NAME)
        FOR_EACH_NET_PHONEYOU_LULU_BB_ENTITIES_MESSAGE_TEMPLATE(REGISTER_TYPE_WITH_NAME)
        FOR_EACH_NET_PHONEYOU_LULU_BB_SINGLETONDESCRIPTORS_MESSAGE_TEMPLATE(REGISTER_TYPE_WITH_NAME)

        qmlRegisterType<EntityCast>(uri, versionMajor, versionMinor, "EntityCast");
        qmlRegisterSingletonType<EntityCastSingleton>(uri, versionMajor, versionMinor, "Cast", entity_cast_provider<EntityCastSingleton>);
        qmlRegisterSingletonType<GlobalSingleton>(uri, versionMajor, versionMinor, "Global", accessor_singleton_provider<GlobalSingleton>);
    }
};

#include "plugin.moc"
