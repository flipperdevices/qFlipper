#include "application.h"

#include <QLocale>
#include <QTranslator>
#include <QQmlContext>
#include <QQuickWindow>
#include <QFontDatabase>
#include <QtQuickControls2/QQuickStyle>

#include "qflipperbackend.h"
#include "updateregistry.h"
#include "screencanvas.h"
#include "preferences.h"

#include "macros.h"

Application::Application(int &argc, char **argv):
    QApplication(argc, argv)
{
    debug_msg(QString("%1 version %2 commit %3.").arg(APP_NAME, APP_VERSION, APP_COMMIT));

    initQmlTypes();
    initContextProperties();
    initTranslations();
    initImports();
    initStyles();
    initFonts();
    initGUI();
}

const QString Application::commitNumber()
{
    return APP_COMMIT;
}

AppUpdater *Application::updater()
{
    return &m_updater;
}

void Application::initStyles()
{
    QQuickWindow::setDefaultAlphaBuffer(true);
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
    QQuickStyle::setStyle(":/style");
}

void Application::initContextProperties()
{
    m_engine.rootContext()->setContextProperty("app", this);
    m_engine.rootContext()->setContextProperty("preferences", globalPrefs());
    m_engine.rootContext()->setContextProperty("deviceRegistry", &m_backend.deviceRegistry);
    m_engine.rootContext()->setContextProperty("firmwareUpdates", &m_backend.firmwareUpdates);
    m_engine.rootContext()->setContextProperty("applicationUpdates", &m_backend.applicationUpdates);
}

void Application::initTranslations()
{
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    for (const QString &locale : uiLanguages) {
        if (translator.load(":/i18n/" + QLocale(locale).name())) {
            installTranslator(&translator);
            break;
        }
    }
}

void Application::initQmlTypes()
{
    qmlRegisterType<ScreenCanvas>("QFlipper", 1, 0, "ScreenCanvas");
    qmlRegisterType<AppUpdater>("QFlipper", 1, 0, "AppUpdater");
}

void Application::initImports()
{
    m_engine.addImportPath(":/imports");
}

void Application::initFonts()
{
    QFontDatabase::addApplicationFont(":/assets/fonts/haxrcorp-4089.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Born2bSportyV2.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/ProggySquare.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/ShareTechMono-Regular.ttf");
}

void Application::initGUI()
{
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    const auto onObjectCreated = [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    };

    connect(&m_engine, &QQmlApplicationEngine::objectCreated, this, onObjectCreated, Qt::QueuedConnection);
    m_engine.load(url);
}
