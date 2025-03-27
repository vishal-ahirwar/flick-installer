#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>
#include<utild.h>
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    Utild util(&app);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("DownloadManager",&util);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("RC", "Main");

    return app.exec();
}

