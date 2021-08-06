#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuickControls2/QQuickStyle>

#include <QLocale>
#include <QTranslator>
#include <QQmlContext>

#include "qflipperbackend.h"
#include "screencanvas.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("qFlipper");
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName("Flipper Devices Inc");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    for (const QString &locale : uiLanguages) {
        if (translator.load(":/i18n/" + QLocale(locale).name())) {
            app.installTranslator(&translator);
            break;
        }
    }

    QFlipperBackend backend;
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("deviceRegistry", &backend.deviceRegistry);
    engine.rootContext()->setContextProperty("updateRegistry", &backend.updateRegistry);
    engine.rootContext()->setContextProperty("downloader", &backend.downloader);

    qmlRegisterType<ScreenCanvas>("QFlipper", 1, 0, "ScreenCanvas");

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    QQuickStyle::setStyle("Universal");
    engine.load(url);

    return app.exec();
}
