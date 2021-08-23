#include "application.h"

#include <QLocale>
#include <QTranslator>
#include <QQmlContext>
#include <QtQuickControls2/QQuickStyle>

#include "application.h"
#include "qflipperbackend.h"
#include "screencanvas.h"

#include "macros.h"

Application::Application(int &argc, char **argv):
    QApplication(argc, argv)
{
    debug_msg(QString("%1 version %2 commit %3.").arg(APP_NAME, APP_VERSION, APP_COMMIT));

    initQmlTypes();
    initInstanceProperties();
    initContextProperties();
    initTranslations();
    initStyles();
    initGUI();
}

const QString Application::commitNumber()
{
    return APP_COMMIT;
}

void Application::initStyles()
{
    QQuickStyle::setStyle("Universal");
}

void Application::initContextProperties()
{
    m_engine.rootContext()->setContextProperty("app", this);
    m_engine.rootContext()->setContextProperty("deviceRegistry", &m_backend.deviceRegistry);
    m_engine.rootContext()->setContextProperty("firmwareUpdates", &m_backend.firmwareUpdates);
    m_engine.rootContext()->setContextProperty("applicationUpdates", &m_backend.applicationUpdates);
    m_engine.rootContext()->setContextProperty("downloader", &m_backend.downloader);
}

void Application::initInstanceProperties()
{
    setApplicationName(APP_NAME);
    setApplicationVersion(APP_VERSION);
    setOrganizationName("Flipper Devices Inc");
    setOrganizationDomain("flipperdevices.com");
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
