#include "tool.h"

#include <QDebug>
#include <QLoggingCategory>

#include "logger.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"

Q_LOGGING_CATEGORY(CAT_TOOL, "TOOL")

Tool::Tool(int argc, char *argv[]):
    QCoreApplication(argc, argv)
{
    initConnections();
    initLogger();
}

Tool::~Tool()
{}

void Tool::onDefaultDeviceChanged()
{
    auto *device = m_backend.device();
    if(device) {
        qCInfo(CAT_TOOL) << "Current device changed to:" << device->deviceState()->deviceInfo().name;
    } else {
        qCCritical(CAT_TOOL) << "All devices disconnected. Exiting.";
        exit(-1);
    }
}

void Tool::onBackendStateChanged()
{
    const auto state = m_backend.backendState();
    if(state == ApplicationBackend::BackendState::ErrorOccured) {
        qCCritical(CAT_TOOL) << "An error has occured:" << m_backend.errorType() << "Exiting.";
        exit(-1);
    }
}

void Tool::initConnections()
{
    connect(&m_backend, &ApplicationBackend::currentDeviceChanged, this, &Tool::onDefaultDeviceChanged);
    connect(&m_backend, &ApplicationBackend::backendStateChanged, this, &Tool::onBackendStateChanged);
}

void Tool::initLogger()
{
    qInstallMessageHandler(Logger::messageOutput);
    globalLogger->setLogLevel(Logger::Terse);
}
