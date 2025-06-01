#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>
#include "utild.h"
int main(int argc, char *argv[])
{
    qDebug() << "Using Qt version:" << QT_VERSION_STR;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
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
    engine.load(url);

    return app.exec();
}

