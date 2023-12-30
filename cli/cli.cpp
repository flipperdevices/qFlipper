#include "cli.h"

#include <QDebug>
#include <QLoggingCategory>

#include "logger.h"
#include "preferences.h"
#include "deviceregistry.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"

Q_LOGGING_CATEGORY(LOG_CLI, "CLI")

Cli::Cli(int &argc, char *argv[]):
    QCoreApplication(argc, argv),
    m_pendingOperation(NoOperation)
{
    m_backend.setOperatingMode(ApplicationBackend::OperatingMode::Cli);

    initConnections();
    initLogger();
    initParser();

    processOptions();
    processArguments();

    qCInfo(LOG_CLI) << "Waiting for devices...";
}

Cli::~Cli()
{}

void Cli::onBackendStateChanged()
{
    const auto state = m_backend.backendState();
    if(state == ApplicationBackend::BackendState::ErrorOccured) {
        qCCritical(LOG_CLI).nospace() << "An error has occurred: " << m_backend.errorType() << ". Exiting.";
        std::exit(EXIT_FAILURE);

    } else if(state == ApplicationBackend::BackendState::WaitingForDevices) {
        qCCritical(LOG_CLI) << "All devices disconnected. Exiting.";

        std::exit(EXIT_SUCCESS);

    } else if(state == ApplicationBackend::BackendState::Ready) {
        // Start the pending operation as soon as the device is ready...
        if(m_pendingOperation != Update) {
            startPendingOperation();
            return;

        } else if(m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::ErrorOccured) {
            qCCritical(LOG_CLI) << "Failed to get firmware updates. Exiting.";
            std::exit(EXIT_FAILURE);
        }

        const auto isFirmwareReady = m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Checking &&
                                     m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Unknown;
        if(isFirmwareReady) {
            startPendingOperation();
        } else {
            // ... But there are special cases when we might have to wait for the firmware update to become available.
            connect(&m_backend, &ApplicationBackend::firmwareUpdateStateChanged, this, &Cli::onUpdateStateChanged);
        }

    } else if(state == ApplicationBackend::BackendState::Finished) {
        m_backend.finalizeOperation();

        qCInfo(LOG_CLI) << "All done! Thank you.";
        std::exit(EXIT_SUCCESS);
    }
}

void Cli::onUpdateStateChanged()
{
    if(m_backend.firmwareUpdateState() == ApplicationBackend::FirmwareUpdateState::ErrorOccured) {
        qCCritical(LOG_CLI) << "Failed to get firmware updates. Exiting.";
        return exit(EXIT_FAILURE);
    }

    const auto isFirmwareReady = m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Checking &&
                                     m_backend.firmwareUpdateState() != ApplicationBackend::FirmwareUpdateState::Unknown;
    if(isFirmwareReady) {
        disconnect(&m_backend, &ApplicationBackend::firmwareUpdateStateChanged, this, &Cli::onUpdateStateChanged);
        startPendingOperation();
    }
}

void Cli::initConnections()
{
    connect(&m_backend, &ApplicationBackend::backendStateChanged, this, &Cli::onBackendStateChanged, Qt::QueuedConnection);
}

void Cli::initLogger()
{
    qInstallMessageHandler(Logger::messageOutput);
    globalLogger->setLogLevel(Logger::Terse);
}

void Cli::initParser()
{
    m_parser.addPositionalArgument(QStringLiteral("list"), QStringLiteral("List all connected Devices"), QStringLiteral("{list,"));
    m_parser.addPositionalArgument(QStringLiteral("update"), QStringLiteral("Perform a Regular Update"), QStringLiteral("update,"));
    m_parser.addPositionalArgument(QStringLiteral("backup"), QStringLiteral("Backup Internal Memory contents"), QStringLiteral("backup <backup_file>,"));
    m_parser.addPositionalArgument(QStringLiteral("restore"), QStringLiteral("Restore Internal Memory contents"), QStringLiteral("restore <backup_file>,"));
    m_parser.addPositionalArgument(QStringLiteral("erase"), QStringLiteral("Erase Internal Memory contents"), QStringLiteral("erase,"));
    m_parser.addPositionalArgument(QStringLiteral("firmware"), QStringLiteral("Flash Core1 Firmware"), QStringLiteral("firmware <firmware_file.dfu>,"));
    m_parser.addPositionalArgument(QStringLiteral("core2radio"), QStringLiteral("Flash Core2 Radio stack"), QStringLiteral("core2radio <firmware_file.bin>,"));
    m_parser.addPositionalArgument(QStringLiteral("core2fus"), QStringLiteral("Flash Core2 Firmware Update Service"), QStringLiteral("core2fus <firmware_file.bin> <target_address>}"));

    m_options.append(QCommandLineOption({QStringLiteral("n"), QStringLiteral("name")}, QStringLiteral("Specify device name"), QStringLiteral("name")));
    m_options.append(QCommandLineOption({QStringLiteral("d"), QStringLiteral("debug-level")}, QStringLiteral("0 - Errors Only, 1 - Terse, 2 - Full"), QStringLiteral("level"), QStringLiteral("1")));
    m_options.append(QCommandLineOption({QStringLiteral("c"), QStringLiteral("update-channel")}, QStringLiteral("Update channel for Firmware Update/Repair"), QStringLiteral("channel"), globalPrefs->firmwareUpdateChannel()));

    m_parser.addOptions(m_options);
    m_parser.addVersionOption();
    m_parser.addHelpOption();

    m_parser.process(*this);
}

void Cli::processOptions()
{
    processDeviceNameOption();
    processDebugLevelOption();
    processUpdateChannelOption();
}

void Cli::processArguments()
{
    const auto args = m_parser.positionalArguments();

    if(args.isEmpty()) {
        m_parser.showHelp(EXIT_SUCCESS);
    } else if(args.startsWith(QStringLiteral("list"))) {
        beginList();
    } else if(args.startsWith(QStringLiteral("update"))) {
        beginUpdate();
    } else if(args.startsWith(QStringLiteral("backup"))) {
        beginBackup();
    } else if(args.startsWith(QStringLiteral("restore"))) {
        beginRestore();
    } else if(args.startsWith(QStringLiteral("erase"))) {
        beginErase();
    } else if(args.startsWith(QStringLiteral("firmware"))) {
        beginFirmware();
    } else if(args.startsWith(QStringLiteral("core2radio"))) {
        beginCore2Radio();
    } else if(args.startsWith(QStringLiteral("core2fus"))) {
        beginCore2FUS();
    } else {
        m_parser.showHelp(EXIT_FAILURE);
    }
}

void Cli::processDebugLevelOption()
{
    const auto &debugLevelOption = m_options[DebugLevelOption];

    if(!m_parser.isSet(debugLevelOption)) {
        return;
    }

    bool canConvert;
    const auto num = m_parser.value(debugLevelOption).toInt(&canConvert);

    if(!canConvert || (num < 0 || num > 2)) {
        qCCritical(LOG_CLI) << "Debug level must be one of the following values: 0, 1, 2.";
        std::exit(EXIT_FAILURE);
    }

    globalLogger->setLogLevel((Logger::LogLevel)num);
}

void Cli::processDeviceNameOption()
{
    const auto &deviceNameOption = m_options[DeviceNameOption];

    if(!m_parser.isSet(deviceNameOption)) {
        return;
    }

    const auto deviceNames = m_parser.value(deviceNameOption).split(',');
    m_backend.deviceRegistry()->setDeviceFilter(deviceNames);
}

void Cli::processUpdateChannelOption()
{
    const auto &updateChannelOption = m_options[UpdateChannelOption];

    if(!m_parser.isSet(updateChannelOption)) {
        return;
    }

    static const QStringList allowedChannelNames = {
        QStringLiteral("release"),
        QStringLiteral("release-candidate"),
        QStringLiteral("development")
    };

    const auto channelName = m_parser.value(updateChannelOption);

    if(!allowedChannelNames.contains(channelName)) {
        qCCritical(LOG_CLI) << "Unknown update channel. Possible channels are:" << allowedChannelNames;
        std::exit(EXIT_FAILURE);
    }

    globalPrefs->setFirmwareUpdateChannel(channelName);
}

void Cli::beginList()
{
    verifyArgumentCount(1);
    qCInfo(LOG_CLI) << "Listing connected devices...";
    m_pendingOperation = List;

    connect(m_backend.deviceRegistry(), &Flipper::DeviceRegistry::deviceCountChanged, this, [=]() {
        const auto *device = m_backend.deviceRegistry()->mostRecentDevice();

        if(device == nullptr) {
            return;
        }

        qCInfo(LOG_CLI) << "Device:" << device->deviceState()->deviceInfo().name
                            << "in" << (device->deviceState()->isRecoveryMode() ? "RECOVERY" : "NORMAL")
                            << "mode";
    });
}

void Cli::beginUpdate()
{
    verifyArgumentCount(1);
    qCInfo(LOG_CLI) << "Performing full firmware update...";
    m_pendingOperation = Update;
}

void Cli::beginBackup()
{
    verifyArgumentCount(2);
    m_fileParameter = QUrl::fromLocalFile(m_parser.positionalArguments().at(1));

    qCInfo(LOG_CLI).noquote().nospace() << "Performing internal storage backup to " << m_fileParameter.toLocalFile() << "...";
    m_pendingOperation = Backup;
}

void Cli::beginRestore()
{
    verifyArgumentCount(2);
    m_fileParameter = QUrl::fromLocalFile(m_parser.positionalArguments().at(1));

    qCInfo(LOG_CLI).noquote().nospace() << "Performing internal restore from " << m_fileParameter.toLocalFile() << "...";
    m_pendingOperation = Restore;
}

void Cli::beginErase()
{
    verifyArgumentCount(1);
    qCInfo(LOG_CLI) << "Performing device factory reset...";
    m_pendingOperation = Erase;
}

void Cli::beginFirmware()
{
    verifyArgumentCount(2);
    const auto arg = m_parser.positionalArguments().at(1);

    if(!arg.endsWith(QStringLiteral(".dfu"), Qt::CaseInsensitive)) {
        qCCritical(LOG_CLI) << "Please provide a firmware file in DFUse format.";
        std::exit(EXIT_FAILURE);
    }

    m_fileParameter = QUrl::fromLocalFile(arg);

    qCInfo(LOG_CLI).noquote().nospace() << "Performing Firmware installation from " << m_fileParameter.toLocalFile() << "...";
    m_pendingOperation = Firmware;
}

void Cli::beginCore2Radio()
{
    verifyArgumentCount(2);
    const auto arg = m_parser.positionalArguments().at(1);

    if(!arg.endsWith(QStringLiteral(".bin"), Qt::CaseInsensitive)) {
        qCCritical(LOG_CLI) << "Please provide a firmware file in .bin format.";
        std::exit(EXIT_FAILURE);
    }

    m_fileParameter = QUrl::fromLocalFile(arg);

    qCInfo(LOG_CLI).noquote().nospace() << "Performing Radio Firmware installation from " << m_fileParameter.toLocalFile() << "...";
    m_pendingOperation = Core2Radio;
}

void Cli::beginCore2FUS()
{
    verifyArgumentCount(3);

    const auto args = m_parser.positionalArguments();
    const auto &arg1 = args[1];
    const auto &arg2 = args[2];

    if(!arg1.endsWith(QStringLiteral(".bin"), Qt::CaseInsensitive)) {
        qCCritical(LOG_CLI) << "Please provide a firmware file in .bin format.";
        std::exit(EXIT_FAILURE);
    }

    m_fileParameter = QUrl::fromLocalFile(arg1);

    bool canConvert;
    m_core2Address = arg2.toULong(&canConvert, 16);

    if(!canConvert) {
        qCCritical(LOG_CLI) << "Please provide a valid hexadecimal address.";
        std::exit(EXIT_FAILURE);
    }

    m_pendingOperation = Core2FUS;
}

void Cli::startPendingOperation()
{
    if(m_pendingOperation == List) {
        // Nothing!
    } else if(m_pendingOperation == Update) {
        m_backend.mainAction();
    } else if(m_pendingOperation == Backup) {
        m_backend.createBackup(m_fileParameter);
    } else if(m_pendingOperation == Restore) {
        m_backend.restoreBackup(m_fileParameter);
    } else if(m_pendingOperation == Erase) {
        m_backend.factoryReset();
    } else if(m_pendingOperation == Firmware) {
        m_backend.installFirmware(m_fileParameter);
    } else if(m_pendingOperation == Core2Radio) {
        m_backend.installWirelessStack(m_fileParameter);
    } else if(m_pendingOperation == Core2FUS) {
        m_backend.installFUS(m_fileParameter, m_core2Address);
    } else {
        qCCritical(LOG_CLI) << "Unhandled operation. Probably a bug!";
        exit(EXIT_FAILURE);
    }
}

void Cli::verifyArgumentCount(int num)
{
    const auto argCount = m_parser.positionalArguments().size();

    if(argCount != num) {
        qCCritical(LOG_CLI).nospace() << "Expected " << num << " arguments, got " << argCount << ". Exiting.";
        std::exit(EXIT_FAILURE);
    }
}
