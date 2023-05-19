#include "legacyupdateoperation.h"

#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QTemporaryFile>
#include <QLoggingCategory>

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

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEFAULT)

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
    if(operationState() == Ready) {
        setOperationState(FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == FetchingFirmware) {
        setOperationState(SavingBackup);
        saveBackup();

    } else if(operationState() == SavingBackup) {
        setOperationState(StartingRecovery);
        startRecovery();

    } else if(operationState() == StartingRecovery) {

        if(m_helper->hasRadioUpdate()) {
            setOperationState(SettingBootMode);
            setBootMode();

        } else{
            setOperationState(DownloadingRadioFirmware);
            advanceOperationState();
        }

    } else if(operationState() == SettingBootMode) {
        setOperationState(DownloadingRadioFirmware);
        downloadRadioFirmware();

    } else if(operationState() == DownloadingRadioFirmware) {
        setOperationState(DownloadingFirmware);
        downloadFirmware();

    } else if(operationState() == DownloadingFirmware) {

        if(m_helper->hasRadioUpdate()) {
            setOperationState(CorrectingOptionBytes);
            correctOptionBytes();

        } else {
            setOperationState(ExitingRecovery);
            exitRecovery();
        }

    } else if((operationState() == ExitingRecovery) ||
              (operationState() == CorrectingOptionBytes)) {
        setOperationState(DownloadingAssets);
        downloadAssets();

    } else if(operationState() == DownloadingAssets) {
        setOperationState(ProvisioningRegion);
        provisionRegion();

    } else if(operationState() == ProvisioningRegion) {
        setOperationState(RestoringBackup);
        restoreBackup();

    } else if(operationState() == RestoringBackup) {
        setOperationState(RestartingDevice);
        restartDevice();

    } else if(operationState() == RestartingDevice) {
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
    if(operationState() == DownloadingRadioFirmware) {
        qCInfo(CATEGORY_DEFAULT) << operation->description() << "failed with reason:" << operation->errorString() << "Attempting to install the firmware anyway...";
        advanceOperationState();

    } else {
        const auto keepError = operationState() == SavingBackup || operationState() == StartingRecovery;
        finishWithError(keepError ? operation->error() : BackendError::OperationError, operation->errorString());
    }
}
