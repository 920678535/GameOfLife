#include <QDir>
#include <QFont>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // TODO Logs

    QGuiApplication app(argc, argv);

    // Font
    app.setFont(QFont("微软雅黑"));
    app.setWindowIcon(QIcon(":/img/logo.png"));

    // Regedit
    app.setOrganizationName("LuLU");
    app.setOrganizationDomain("gameoflife.phoneyou.net");
    app.setApplicationName("GAME OF LIFE");
    app.setApplicationVersion("1.0.0");

    // Translate
    QTranslator appTranslator;
    appTranslator.load(QLocale(), "show", "_", ":/i18n/");
    QCoreApplication::installTranslator(&appTranslator);

    // TODO Arguments

    // TODO Connect the bb_board

    QQmlApplicationEngine engine;
    // TODO AddImportPath
    QDir _amsterImportPath(qgetenv("AMSTERFWK"));
    if (_amsterImportPath.cd("qml")) {
        engine.addImportPath(_amsterImportPath.path());
    }
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    // TODO Disconnect the bb_board


    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
