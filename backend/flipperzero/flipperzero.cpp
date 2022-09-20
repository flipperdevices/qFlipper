#include "flipperzero.h"

#include <QUrl>
#include <QDebug>
#include <QTimer>
#include <QLoggingCategory>

#include "preferences.h"
#include "flipperupdates.h"

#include "devicestate.h"

#include "protobufsession.h"
#include "utilityinterface.h"
#include "recoveryinterface.h"

#include "toplevel/wirelessstackupdateoperation.h"
#include "toplevel/firmwareinstalloperation.h"
#include "toplevel/settingsrestoreoperation.h"
#include "toplevel/settingsbackupoperation.h"
#include "toplevel/fullupdateoperation.h"
#include "toplevel/factoryresetoperation.h"
#include "toplevel/fullrepairoperation.h"
#include "toplevel/legacyupdateoperation.h"

#include "utility/storageinforefreshoperation.h"

Q_LOGGING_CATEGORY(CAT_DEVICE, "DEV")

#define CHANNEL_DEVELOPMENT "development"
#define CHANNEL_RELEASE_CANDIDATE "release-candidate"
#define CHANNEL_RELEASE "release"

using namespace Flipper;
using namespace Updates;
using namespace Zero;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),
    m_state(new DeviceState(info, this)),
    m_rpc(new ProtobufSession(info.portInfo, this)),
    m_recovery(new RecoveryInterface(m_state, this)),
    m_utility(new UtilityInterface(m_state, m_rpc, this))
{
    connect(m_state, &DeviceState::deviceInfoChanged, this, &FlipperZero::onDeviceInfoChanged);
    connect(m_state, &DeviceState::deviceInfoChanged, this, &FlipperZero::deviceStateChanged);

    connect(m_rpc, &ProtobufSession::sessionStateChanged, this, &FlipperZero::onSessionStatusChanged);

    onDeviceInfoChanged();
}

DeviceState *FlipperZero::deviceState() const
{
    return m_state;
}

ProtobufSession *FlipperZero::rpc() const
{
    return m_rpc;
}

UtilityInterface *FlipperZero::utility() const
{
    return m_utility;
}

// TODO: Handle -rcxx suffixes correctly
bool FlipperZero::canUpdate(const Updates::VersionInfo &versionInfo) const
{
    const auto &storageInfo = m_state->deviceInfo().storage;
    const auto &radioStackVersion = m_state->deviceInfo().radioVersion;

    const auto noAssets = storageInfo.isExternalPresent && !storageInfo.isAssetsInstalled;
    const auto noRadioStack = radioStackVersion.isEmpty();

    if(noAssets || noRadioStack) {
        return true;
    }

    static const auto DEVELOPMENT = QStringLiteral("development");
    static const auto RELEASE_CANDIDATE = QStringLiteral("release-candidate");
    static const auto RELEASE = QStringLiteral("release");

    const auto &firmwareInfo = m_state->deviceInfo().firmware;

    const auto &deviceChannel = firmwareInfo.channel;
    const auto &deviceVersion = (deviceChannel == QStringLiteral("development")) ?
                firmwareInfo.commit :
                firmwareInfo.version;

    const auto &deviceDate = firmwareInfo.date;

    const auto &serverChannel = globalPrefs->firmwareUpdateChannel();
    const auto &serverVersion = versionInfo.number();
    const auto &serverDate = versionInfo.date();

    if(deviceChannel == RELEASE) {
        if(serverChannel == RELEASE) {
            return VersionInfo::compare(deviceVersion, serverVersion) < 0;
        } else if(serverChannel == RELEASE_CANDIDATE) {
            return VersionInfo::compare(deviceVersion, serverVersion) < 0;
        } else if(serverChannel == DEVELOPMENT) {
            return deviceDate <= serverDate;
        }

    } else if(deviceChannel == RELEASE_CANDIDATE) {
        if(serverChannel == RELEASE) {
            return VersionInfo::compare(deviceVersion, serverVersion) <= 0;
        } else if(serverChannel == RELEASE_CANDIDATE) {
            return VersionInfo::compare(deviceVersion, serverVersion) < 0;
        } else if(serverChannel == DEVELOPMENT) {
            return deviceDate <= serverDate;
        }

    } else if(deviceChannel == DEVELOPMENT) {
        if(serverChannel == RELEASE) {
            return deviceDate <= serverDate;
        } else if(serverChannel == RELEASE_CANDIDATE) {
            return deviceDate <= serverDate;
        } else if(serverChannel == DEVELOPMENT) {
            return (deviceVersion != serverVersion) && (deviceDate <= serverDate);
        }
    }

    return false;
}

bool FlipperZero::canInstall(const Updates::VersionInfo &versionInfo) const
{
    Q_UNUSED(versionInfo)

    const auto &deviceChannel = m_state->deviceInfo().firmware.channel;
    const auto &serverChannel = globalPrefs->firmwareUpdateChannel();

    return deviceChannel != serverChannel;
}

bool FlipperZero::canRepair(const Updates::VersionInfo &versionInfo) const
{
    Q_UNUSED(versionInfo)
    return m_state->isRecoveryMode();
}

void FlipperZero::fullUpdate(const Updates::VersionInfo &versionInfo)
{
    const auto &storageInfo = m_state->deviceInfo().storage;
    const auto &protobufInfo = m_state->deviceInfo().protobuf;
    const auto isLegacyUpdate = !storageInfo.isExternalPresent || ((protobufInfo.versionMajor == 0) && (protobufInfo.versionMinor < 7));

    if(isLegacyUpdate) {
        registerOperation(new LegacyUpdateOperation(m_recovery, m_utility, m_state, versionInfo, this));
    } else {
        registerOperation(new FullUpdateOperation(m_utility, m_state, versionInfo, this));
    }
}

void FlipperZero::fullRepair(const Updates::VersionInfo &versionInfo)
{
    registerOperation(new FullRepairOperation(m_recovery, m_utility, m_state, versionInfo, this));
}

void FlipperZero::createBackup(const QUrl &backupUrl)
{
    registerOperation(new SettingsBackupOperation(m_utility, m_state, backupUrl, this));
}

void FlipperZero::restoreBackup(const QUrl &backupUrl)
{
    registerOperation(new SettingsRestoreOperation(m_utility, m_state, backupUrl, this));
}

void FlipperZero::factoryReset()
{
    registerOperation(new FactoryResetOperation(m_utility, m_state, this));
}

void FlipperZero::installFirmware(const QUrl &fileUrl)
{
    if(fileUrl.fileName().endsWith(QStringLiteral(".tgz"))) {
        registerOperation(new FullUpdateOperation(m_utility, m_state, fileUrl, this));
    } else {
        registerOperation(new FirmwareInstallOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
    }
}

void FlipperZero::installWirelessStack(const QUrl &fileUrl)
{
    registerOperation(new WirelessStackUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), this));
}

void FlipperZero::installFUS(const QUrl &fileUrl, uint32_t address)
{
    registerOperation(new FUSUpdateOperation(m_recovery, m_utility, m_state, fileUrl.toLocalFile(), address, this));
}

void FlipperZero::refreshStorageInfo()
{
    m_utility->refreshStorageInfo();
}

void FlipperZero::finalizeOperation()
{
    // TODO: write a better implementation that would:
    // 1. Check if the port is open and functional
    // 2. Test if the RPC session is up an running
    // 3. Open RPC session if necessary

    if(m_state->isError()) {
        m_state->clearError();
    }
}

void FlipperZero::onDeviceInfoChanged()
{
    if(m_state->isOnline()) {
        // Most likely Storage info update
        return;
    } else if(m_state->isRecoveryMode()) {
        // Recovery mode, not using Protobuf
        m_state->setOnline(true);
        return;
    }

    // Perform Protobuf session initialisation
    const auto &deviceInfo = m_state->deviceInfo();

    qCDebug(CAT_DEVICE).noquote() << "Version:" << deviceInfo.firmware.version << "commit:"
                                  << deviceInfo.firmware.commit << "radio:" << deviceInfo.radioVersion;

    const auto &pb = deviceInfo.protobuf;
    const auto &pi = deviceInfo.portInfo;

    m_rpc->setMajorVersion(pb.versionMajor);
    m_rpc->setMinorVersion(pb.versionMinor);
    m_rpc->setSerialPort(pi);

    // 100 ms delay to prevent race condition in Flipper
    QTimer::singleShot(100, m_rpc, &ProtobufSession::startSession);
}

void FlipperZero::onSessionStatusChanged()
{
    if(m_rpc->isError()) {
        m_state->setError(m_rpc->error(), m_rpc->errorString());
        emit operationFinished();

    } else if(m_rpc->isSessionUp()) {
        m_state->setOnline(true);
    }
}

void FlipperZero::registerOperation(AbstractOperation *operation)
{
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCCritical(CAT_DEVICE).noquote() << operation->description() << "ERROR:" << operation->errorString();
            m_state->setError(operation->error(), operation->errorString());

        } else {
            qCInfo(CAT_DEVICE).noquote() << operation->description() << "SUCCESS";
        }

        operation->deleteLater();
        emit operationFinished();
    });

    qCInfo(CAT_DEVICE).noquote() << operation->description() << "START";
    operation->start();
}
