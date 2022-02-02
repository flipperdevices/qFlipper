#include "tool.h"

#include <QDebug>
#include <QLoggingCategory>

#include "logger.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"

Q_LOGGING_CATEGORY(LOG_TOOL, "TOOL")

Tool::Tool(int argc, char *argv[]):
    QCoreApplication(argc, argv),
    m_pendingOperation(NoOperation),
    m_repeatCount(1)
{
    initConnections();
    initLogger();
    initParser();

    processOptions();
    processArguments();

    qCInfo(LOG_TOOL) << "Waiting for devices...";
}

Tool::~Tool()
{}

void Tool::onBackendStateChanged()
{
    const auto state = m_backend.backendState();
    if(state == ApplicationBackend::BackendState::ErrorOccured) {
        qCCritical(LOG_TOOL) << "An error has occured:" << m_backend.errorType() << "Exiting.";
        exit(-1);

    } else if(state == ApplicationBackend::BackendState::WaitingForDevices) {
        qCCritical(LOG_TOOL) << "All devices disconnected. Exiting.";
        exit(0);

    } else if(state == ApplicationBackend::BackendState::Ready) {
        startPendingOperation();
    } else if(state == ApplicationBackend::BackendState::Finished) {
        m_backend.finalizeOperation();
    }
}

void Tool::onUpdateStateChanged()
{
    const auto deviceReady = m_backend.backendState() == ApplicationBackend::BackendState::Ready;
    const auto firmwareReady = m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::CanRepair;
    const auto dependsOnFirmware = (m_pendingOperation == FullUpdate) || (m_pendingOperation == FullRepair);

    if(deviceReady && firmwareReady && dependsOnFirmware) {
        startPendingOperation();
    }
}

void Tool::initConnections()
{
    connect(&m_backend, &ApplicationBackend::backendStateChanged, this, &Tool::onBackendStateChanged);
    connect(&m_backend, &ApplicationBackend::firmwareUpdateStateChanged, this, &Tool::onUpdateStateChanged);
}

void Tool::initLogger()
{
    qInstallMessageHandler(Logger::messageOutput);
    globalLogger->setLogLevel(Logger::Terse);
}

void Tool::initParser()
{
    m_parser.addPositionalArgument(QStringLiteral("update"), QStringLiteral("Update Firmware and Assets to latest version"));
    m_parser.addPositionalArgument(QStringLiteral("repair"), QStringLiteral("Repair a broken Firmware installation"));
    m_parser.addPositionalArgument(QStringLiteral("backup"), QStringLiteral("Backup Internal Memory contents"));
    m_parser.addPositionalArgument(QStringLiteral("restore"), QStringLiteral("Restore Internal Memory contents"));
    m_parser.addPositionalArgument(QStringLiteral("erase"), QStringLiteral("Erase Internal Memory contents"));
    m_parser.addPositionalArgument(QStringLiteral("wipe"), QStringLiteral("Wipe entire MCU Flash Memory"));
    m_parser.addPositionalArgument(QStringLiteral("firmware"), QStringLiteral("Flash Core1 Firmware"));
    m_parser.addPositionalArgument(QStringLiteral("core2radio"), QStringLiteral("Flash Core2 Radio stack"));
    m_parser.addPositionalArgument(QStringLiteral("core2fus"), QStringLiteral("Flash Core2 Firmware Update Service"));

    m_options.append(QCommandLineOption({QStringLiteral("d"), QStringLiteral("debug-level")}, QStringLiteral("0 - Errors Only, 1 - Terse, 2 - Full"), QStringLiteral("1")));
    m_options.append(QCommandLineOption({QStringLiteral("n"), QStringLiteral("repeat-number")}, QStringLiteral("Number of times to repeat the operation, 0 - indefinitely"), QStringLiteral("1")));

    m_parser.addOptions(m_options);
    m_parser.addHelpOption();

    m_parser.process(*this);
}

void Tool::processOptions()
{
    processDebugLevelOption();
    processRepeatNumberOption();
}

void Tool::processArguments()
{
    const auto args = m_parser.positionalArguments();

    if(args.startsWith(QStringLiteral("update"))) {
        beginUpdate();
    } else if(args.startsWith(QStringLiteral("repair"))) {
        beginRepair();
    } else {
        m_parser.showHelp(-1);
    }
}

void Tool::processDebugLevelOption()
{
    const auto &debugLevelOption = m_options[0];

    if(!m_parser.isSet(debugLevelOption)) {
        return;
    }

    bool canConvert;
    const auto num = m_parser.value(debugLevelOption).toInt(&canConvert);

    if(!canConvert || (num < 0 || num > 2)) {
        qCCritical(LOG_TOOL) << "Debug level must be one of the following values: 0, 1, 2.";
        std::exit(-1);
    }

    globalLogger->setLogLevel((Logger::LogLevel)num);
}

void Tool::processRepeatNumberOption()
{
    const auto &repeatNumberOption = m_options[1];

    if(!m_parser.isSet(repeatNumberOption)) {
        return;
    }

    bool canConvert;
    const auto num = m_parser.value(repeatNumberOption).toInt(&canConvert);

    if(!canConvert || (num < 0)) {
        qCCritical(LOG_TOOL) << "Repeat number must be a whole non-negative number.";
        std::exit(-1);
    }

    m_repeatCount = num ? num : -1;
}

void Tool::beginUpdate()
{
    qCInfo(LOG_TOOL) << "Performing full firmware update...";

    m_pendingOperation = FullUpdate;

    const auto deviceReady = m_backend.backendState() == ApplicationBackend::BackendState::Ready;
    const auto firmwareReady = (m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::CanInstall) ||
                             (m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::CanUpdate);

    if(deviceReady && firmwareReady) {
        startPendingOperation();
    }
}

void Tool::beginRepair()
{
    qCInfo(LOG_TOOL) << "Performing full firmware repair...";

    m_pendingOperation = FullRepair;

    const auto deviceReady = m_backend.backendState() == ApplicationBackend::BackendState::Ready;
    const auto firmwareReady = m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::CanRepair;

    if(deviceReady && firmwareReady) {
        startPendingOperation();
    }
}

void Tool::startPendingOperation()
{
    if(m_repeatCount == 0) {
        qCCritical(LOG_TOOL) << "All done! Exiting.";
        exit(0);

    } else if(m_repeatCount > 0) {
        --m_repeatCount;
    }

    if((m_pendingOperation == FullUpdate) || (m_pendingOperation == FullRepair)) {
        m_backend.mainAction();

    } else {
        exit(-1);
    }
}
