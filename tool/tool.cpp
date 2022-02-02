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
        // Start the pending operation as soon as the device is ready...
        if(m_pendingOperation != DefaultAction) {
            startPendingOperation();
            return;

        } else if(m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::ErrorOccured) {
            qCCritical(LOG_TOOL) << "Failed to get firmware updates. Exiting.";
            exit(-1);
        }

        const auto isFirmwareReady = m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Checking &&
                                     m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Unknown;
        if(isFirmwareReady) {
            startPendingOperation();
        } else {
            // ... But there are special cases when we might have to wait for the firmware update to become available.
            connect(&m_backend, &ApplicationBackend::firmwareUpdateStateChanged, this, &Tool::onUpdateStateChanged);
        }

    } else if(state == ApplicationBackend::BackendState::Finished) {
        m_backend.finalizeOperation();
    }
}

void Tool::onUpdateStateChanged()
{
    if(m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::ErrorOccured) {
        qCCritical(LOG_TOOL) << "Failed to get firmware updates. Exiting.";
        exit(-1);
    }

    const auto isFirmwareReady = m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Checking &&
                                     m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Unknown;
    if(isFirmwareReady) {
        disconnect(&m_backend, &ApplicationBackend::firmwareUpdateStateChanged, this, &Tool::onUpdateStateChanged);
        startPendingOperation();
    }
}

void Tool::initConnections()
{
    connect(&m_backend, &ApplicationBackend::backendStateChanged, this, &Tool::onBackendStateChanged);
}

void Tool::initLogger()
{
    qInstallMessageHandler(Logger::messageOutput);
    globalLogger->setLogLevel(Logger::Terse);
}

void Tool::initParser()
{
    m_parser.addPositionalArgument(QStringLiteral("backup"), QStringLiteral("Backup Internal Memory contents"));
    m_parser.addPositionalArgument(QStringLiteral("restore"), QStringLiteral("Restore Internal Memory contents"));
    m_parser.addPositionalArgument(QStringLiteral("erase"), QStringLiteral("Erase Internal Memory contents"));
    m_parser.addPositionalArgument(QStringLiteral("wipe"), QStringLiteral("Wipe entire MCU Flash Memory"));
    m_parser.addPositionalArgument(QStringLiteral("firmware"), QStringLiteral("Flash Core1 Firmware"));
    m_parser.addPositionalArgument(QStringLiteral("core2radio"), QStringLiteral("Flash Core2 Radio stack"));
    m_parser.addPositionalArgument(QStringLiteral("core2fus"), QStringLiteral("Flash Core2 Firmware Update Service"));

    m_options.append(QCommandLineOption({QStringLiteral("d"), QStringLiteral("debug-level")}, QStringLiteral("0 - Errors Only, 1 - Terse, 2 - Full"), QStringLiteral("1")));
    m_options.append(QCommandLineOption({QStringLiteral("n"), QStringLiteral("repeat-number")}, QStringLiteral("Number of times to repeat the operation, 0 - indefinitely"), QStringLiteral("1")));

    m_parser.setApplicationDescription(QStringLiteral("A text mode non-interactive qFlipper counterpart. Run without arguments to quickly perform Firmware Update/Repair."));

    m_parser.addOptions(m_options);
    m_parser.addVersionOption();
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

    if(args.isEmpty()) {
        beginDefaultAction();
    } else if(args.startsWith(QStringLiteral("backup"))) {
    } else if(args.startsWith(QStringLiteral("restore"))) {
    } else if(args.startsWith(QStringLiteral("erase"))) {
    } else if(args.startsWith(QStringLiteral("wipe"))) {
    } else if(args.startsWith(QStringLiteral("firmware"))) {
    } else if(args.startsWith(QStringLiteral("core2radio"))) {
    } else if(args.startsWith(QStringLiteral("core2fus"))) {
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

    qCInfo(LOG_TOOL).noquote() << "Will repeat the operation" << (num ? QStringLiteral("%1 times.").arg(num) : QStringLiteral("indefinitely."));

    m_repeatCount = num ? num : -1;
}

void Tool::beginDefaultAction()
{
    qCInfo(LOG_TOOL) << "Performing full firmware update...";
    m_pendingOperation = DefaultAction;
}

void Tool::startPendingOperation()
{
    if(m_repeatCount == 0) {
        qCCritical(LOG_TOOL) << "All done! Thank you.";
        exit(0);

    } else if(m_repeatCount > 0) {
        --m_repeatCount;
    }

    if(m_pendingOperation == DefaultAction) {
        m_backend.mainAction();

    } else if(m_pendingOperation == Backup) {
//        m_backend.createBackup();
    } else if(m_pendingOperation == Restore) {
//        m_backend.restoreBackup();
    } else if(m_pendingOperation == Erase) {
        m_backend.factoryReset();
    } else if(m_pendingOperation == Wipe) {
        qCCritical(LOG_TOOL) << "Wipe is not implemented yet. Sorry!";
        exit(-1);
    } else if(m_pendingOperation == Firmware) {
//        m_backend.installFirmware();
    } else if(m_pendingOperation == Core2Radio) {
//        m_backend.installWirelessStack();
    } else if(m_pendingOperation == Core2FUS) {
//        m_backend.installFUS();
    } else {
        qCCritical(LOG_TOOL) << "Unhandled operation. Probably a bug!";
        exit(-1);
    }
}
