#include "application.h"

#include <QDebug>
#include <QTimer>
#include <QLocale>
#include <QDateTime>
#include <QTranslator>
#include <QQmlContext>
#include <QQuickWindow>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <QtQuickControls2/QQuickStyle>

#include "updateregistry.h"
#include "deviceregistry.h"
#include "screencanvas.h"
#include "preferences.h"
#include "logger.h"

Q_LOGGING_CATEGORY(CATEGORY_APP, "APP")

Application::Application(int &argc, char **argv):
    QApplication(argc, argv),
    m_updateRegistry(globalPrefs->checkApplicationUpdates() ? QStringLiteral("https://update.flipperzero.one/qFlipper/directory.json") : QString()),
    m_dangerFeaturesEnabled(QGuiApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::AltModifier)
{
    initConnections();
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
    if(m_dangerFeaturesEnabled) {
        qCCritical(CATEGORY_APP) << "Dangerous features enabled! Please be careful.";
    }
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

bool Application::isDangerousFeaturesEnabled() const
{
    return m_dangerFeaturesEnabled;
}

bool Application::isUpdateable() const
{
    return globalPrefs->checkApplicationUpdates() && m_updateRegistry.isReady()
            && m_updater.canUpdate(m_updateRegistry.latestVersion());
}

void Application::selfUpdate()
{
    m_updater.installUpdate(m_updateRegistry.latestVersion());
}

void Application::checkForUpdates()
{
    m_updateRegistry.check();
}

void Application::initConnections()
{
    connect(&m_updateRegistry, &Flipper::UpdateRegistry::latestVersionChanged, this, &Application::isUpdateableChanged);
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
    m_engine.rootContext()->setContextProperty("firmwareUpdates", m_backend.firmwareUpdates());
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
    qmlRegisterSingletonInstance("QFlipper", 1, 0, "App", this);
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
