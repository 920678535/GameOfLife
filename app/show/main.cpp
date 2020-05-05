#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // TODO Logs

    QGuiApplication app(argc, argv);

    // Regedit
    app.setOrganizationName("LuLU");
    app.setOrganizationDomain("gameoflife.phoneyou.net");
    app.setApplicationName("GAME OF LIFE");
    app.setApplicationVersion("1.0.0");

    // TODO Translate

    // TODO Arguments

    // TODO Connect the bb_board

    // TODO AddImportPath

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    // TODO Disconnect the bb_board


    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
