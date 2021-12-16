#include "application.h"

#include <QDebug>
#include <QLocale>
#include <QDateTime>
#include <QTranslator>
#include <QQmlContext>
#include <QQuickWindow>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <QtQuickControls2/QQuickStyle>

#include "applicationbackend.h"
#include "updateregistry.h"
#include "screencanvas.h"
#include "preferences.h"
#include "logger.h"

Q_LOGGING_CATEGORY(CATEGORY_APP, "APP")

Application::Application(int &argc, char **argv):
    QApplication(argc, argv),
    m_updater(this),
    m_backend(this),
    m_engine(this)
{
    initLogger();
    initQmlTypes();
    initContextProperties();
    initTranslations();
    initImports();
    initStyles();
    initFonts();
    initGUI();

    qCInfo(CATEGORY_APP).noquote() << APP_NAME << "version" << APP_VERSION << "commit"
                                   << APP_COMMIT << QDateTime::fromSecsSinceEpoch(APP_TIMESTAMP).toString();
}

Application::~Application()
{
    qCInfo(CATEGORY_APP).noquote() << APP_NAME << "exited";
}

const QString Application::commitNumber()
{
    return APP_COMMIT;
}

ApplicationUpdater *Application::updater()
{
    return &m_updater;
}

void Application::initLogger()
{
    qInstallMessageHandler(Logger::messageOutput);
}

void Application::initStyles()
{
    QQuickWindow::setDefaultAlphaBuffer(true);
    QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
    QQuickStyle::setStyle(":/style");
}

void Application::initContextProperties()
{
    //TODO: Replace context properties with QML singletons
    m_engine.rootContext()->setContextProperty("app", this);
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
    qmlRegisterType<ApplicationUpdater>("QFlipper", 1, 0, "AppUpdater");

    qmlRegisterSingletonInstance("QFlipper", 1, 0, "Logger", globalLogger);
    qmlRegisterSingletonInstance("QFlipper", 1, 0, "Preferences", globalPrefs);
    qmlRegisterSingletonInstance("QFlipper", 1, 0, "Backend", &m_backend);
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
