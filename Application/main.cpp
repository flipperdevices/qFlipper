#include <QApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>
#include <QQmlContext>

#include <QThread>
#include <QDebug>
#include <QFile>

#include "flipartnerbackend.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    for (const QString &locale : uiLanguages) {
        const QString baseName = "Application_" + QLocale(locale).name();

        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    FlipartnerBackend backend;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("deviceRegistry", &backend.deviceRegistry);
    engine.rootContext()->setContextProperty("updateRegistry", &backend.updateRegistry);
    engine.rootContext()->setContextProperty("downloader", &backend.downloader);

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    qDebug() << "Main thread started with id" << QThread::currentThreadId();

    return app.exec();
}
