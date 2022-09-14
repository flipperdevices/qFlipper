#include "legacyupdateoperation.h"

#include <QFile>
#include <QDateTime>
#include <QTemporaryFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"
#include "flipperzero/utility/regionprovisioningoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/correctoptionbytesoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "flipperzero/helper/firmwarehelper.h"

#include "tempdirectories.h"

#define SHIPPED_VERSION QStringLiteral("0.43.1") // Old version that is shipped by default

using namespace Flipper;
using namespace Zero;

LegacyUpdateOperation::LegacyUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_versionInfo(versionInfo),
    m_skipBackup(deviceState()->deviceInfo().firmware.version == SHIPPED_VERSION),
    m_backupUrl(globalTempDirs->fileUrl(QStringLiteral("%1-%2.tgz").arg(state->deviceInfo().name, QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"))))
{}

const QString LegacyUpdateOperation::description() const
{
    return QStringLiteral("Legacy Update @%1").arg(deviceState()->name());
}

void LegacyUpdateOperation::nextStateLogic()
{
    if(operationState() == LegacyUpdateOperation::Ready) {
        setOperationState(LegacyUpdateOperation::FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == LegacyUpdateOperation::FetchingFirmware) {
        setOperationState(LegacyUpdateOperation::SavingBackup);
        saveBackup();

    } else if(operationState() == LegacyUpdateOperation::SavingBackup) {
        setOperationState(LegacyUpdateOperation::StartingRecovery);
        startRecovery();

    } else if(operationState() == LegacyUpdateOperation::StartingRecovery) {

        if(m_helper->hasRadioUpdate()) {
            setOperationState(LegacyUpdateOperation::SettingBootMode);
            setBootMode();

        } else{
            setOperationState(LegacyUpdateOperation::DownloadingRadioFirmware);
            advanceOperationState();
        }

    } else if(operationState() == LegacyUpdateOperation::SettingBootMode) {
        setOperationState(LegacyUpdateOperation::DownloadingRadioFirmware);
        downloadRadioFirmware();

    } else if(operationState() == LegacyUpdateOperation::DownloadingRadioFirmware) {
        setOperationState(LegacyUpdateOperation::DownloadingFirmware);
        downloadFirmware();

    } else if(operationState() == LegacyUpdateOperation::DownloadingFirmware) {

        if(m_helper->hasRadioUpdate()) {
            setOperationState(LegacyUpdateOperation::CorrectingOptionBytes);
            correctOptionBytes();

        } else {
            setOperationState(LegacyUpdateOperation::ExitingRecovery);
            exitRecovery();
        }

    } else if((operationState() == LegacyUpdateOperation::ExitingRecovery) ||
              (operationState() == LegacyUpdateOperation::CorrectingOptionBytes)) {
        setOperationState(LegacyUpdateOperation::DownloadingAssets);
        downloadAssets();

    } else if(operationState() == LegacyUpdateOperation::DownloadingAssets) {
        setOperationState(LegacyUpdateOperation::ProvisioningRegion);
        provisionRegion();

    } else if(operationState() == LegacyUpdateOperation::ProvisioningRegion) {
        setOperationState(LegacyUpdateOperation::RestoringBackup);
        restoreBackup();

    } else if(operationState() == LegacyUpdateOperation::RestoringBackup) {
        setOperationState(LegacyUpdateOperation::RestartingDevice);
        restartDevice();

    } else if(operationState() == LegacyUpdateOperation::RestartingDevice) {
        finish();
    }
}

void LegacyUpdateOperation::fetchFirmware()
{
    m_helper = new FirmwareHelper(deviceState(), m_versionInfo, this);

    connect(m_helper, &AbstractOperationHelper::finished, this, [=]() {
        if(m_helper->isError()) {
            finishWithError(m_helper->error(), QStringLiteral("Failed to fetch the files: %1").arg(m_helper->errorString()));
        } else {
            advanceOperationState();
        }
    });
}

void LegacyUpdateOperation::saveBackup()
{
    if(m_skipBackup) {
        advanceOperationState();
    } else {
        registerSubOperation(m_utility->backupInternalStorage(m_backupUrl));
    }
}

void LegacyUpdateOperation::startRecovery()
{
    registerSubOperation(m_utility->startRecoveryMode());
}

void LegacyUpdateOperation::setBootMode()
{
    registerSubOperation(m_recovery->setRecoveryBootMode());
}

void LegacyUpdateOperation::downloadFirmware()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::Firmware);
    registerSubOperation(m_recovery->downloadFirmware(file));
}

void LegacyUpdateOperation::downloadRadioFirmware()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::RadioFirmware);
    registerSubOperation(m_recovery->downloadWirelessStack(file));
}

void LegacyUpdateOperation::correctOptionBytes()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::OptionBytes);
    registerSubOperation(m_recovery->fixOptionBytes(file));
}

void LegacyUpdateOperation::exitRecovery()
{
    registerSubOperation(m_recovery->exitRecoveryMode());
}

void LegacyUpdateOperation::downloadAssets()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::AssetsTgz);
    registerSubOperation(m_utility->downloadAssets(file));
}

void LegacyUpdateOperation::provisionRegion()
{
    registerSubOperation(m_utility->provisionRegionData());
}

void LegacyUpdateOperation::restoreBackup()
{
    if(m_skipBackup) {
        advanceOperationState();
    } else {
        registerSubOperation(m_utility->restoreInternalStorage(m_backupUrl));
    }
}

void LegacyUpdateOperation::restartDevice()
{
    if(m_skipBackup && deviceState()->deviceInfo().storage.isAssetsInstalled) {
        advanceOperationState();
    } else {
        registerSubOperation(m_utility->restartDevice());
    }
}

void LegacyUpdateOperation::onSubOperationError(AbstractOperation *operation)
{
    const auto keepError = operationState() == LegacyUpdateOperation::SavingBackup ||
                           operationState() == LegacyUpdateOperation::StartingRecovery;

    finishWithError(keepError ? operation->error() : BackendError::OperationError, operation->errorString());
}
