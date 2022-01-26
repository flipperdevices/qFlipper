#include "fullupdateoperation.h"

#include <QFile>
#include <QTemporaryFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/correctoptionbytesoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "flipperzero/helper/firmwarehelper.h"

#include "tempdirectories.h"

using namespace Flipper;
using namespace Zero;

FullUpdateOperation::FullUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_versionInfo(versionInfo)
{}

const QString FullUpdateOperation::description() const
{
    return QStringLiteral("Full Update @%1").arg(deviceState()->name());
}

void FullUpdateOperation::nextStateLogic()
{
    if(operationState() == FullUpdateOperation::Ready) {
        setOperationState(FullUpdateOperation::FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == FullUpdateOperation::FetchingFirmware) {
        setOperationState(FullUpdateOperation::SavingBackup);
        saveBackup();

    } else if(operationState() == FullUpdateOperation::SavingBackup) {
        setOperationState(FullUpdateOperation::StartingRecovery);
        startRecovery();

    } else if(operationState() == FullUpdateOperation::StartingRecovery) {

        if(m_helper->hasRadioUpdate()) {
            setOperationState(FullUpdateOperation::SettingBootMode);
            setBootMode();

        } else{
            setOperationState(FullUpdateOperation::DownloadingRadioFirmware);
            advanceOperationState();
        }

    } else if(operationState() == FullUpdateOperation::SettingBootMode) {
        setOperationState(FullUpdateOperation::DownloadingRadioFirmware);
        downloadRadioFirmware();

    } else if(operationState() == FullUpdateOperation::DownloadingRadioFirmware) {
        setOperationState(FullUpdateOperation::DownloadingFirmware);
        downloadFirmware();

    } else if(operationState() == FullUpdateOperation::DownloadingFirmware) {

        if(m_helper->hasRadioUpdate()) {
            setOperationState(FullUpdateOperation::CorrectingOptionBytes);
            correctOptionBytes();

        } else {
            setOperationState(FullUpdateOperation::ExitingRecovery);
            exitRecovery();
        }

    } else if((operationState() == FullUpdateOperation::ExitingRecovery) ||
              (operationState() == FullUpdateOperation::CorrectingOptionBytes)) {
        setOperationState(FullUpdateOperation::DownloadingAssets);
        downloadAssets();

    } else if(operationState() == FullUpdateOperation::DownloadingAssets) {
        setOperationState(FullUpdateOperation::RestoringBackup);
        restoreBackup();

    } else if(operationState() == FullUpdateOperation::RestoringBackup) {
        setOperationState(FullUpdateOperation::RestartingDevice);
        restartDevice();

    } else if(operationState() == FullUpdateOperation::RestartingDevice) {
        finish();
    }
}

void FullUpdateOperation::fetchFirmware()
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

void FullUpdateOperation::saveBackup()
{
    registerSubOperation(m_utility->backupInternalStorage(globalTempDirs->root().absolutePath()));
}

void FullUpdateOperation::startRecovery()
{
    registerSubOperation(m_utility->startRecoveryMode());
}

void FullUpdateOperation::setBootMode()
{
    registerSubOperation(m_recovery->setRecoveryBootMode());
}

void FullUpdateOperation::downloadFirmware()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::Firmware);
    registerSubOperation(m_recovery->downloadFirmware(file));
}

void FullUpdateOperation::downloadRadioFirmware()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::RadioFirmware);
    registerSubOperation(m_recovery->downloadWirelessStack(file));
}

void FullUpdateOperation::correctOptionBytes()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::OptionBytes);
    registerSubOperation(m_recovery->fixOptionBytes(file));
}

void FullUpdateOperation::exitRecovery()
{
    registerSubOperation(m_recovery->exitRecoveryMode());
}

void FullUpdateOperation::downloadAssets()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::AssetsTgz);
    registerSubOperation(m_utility->downloadAssets(file));
}

void FullUpdateOperation::restoreBackup()
{
    registerSubOperation(m_utility->restoreInternalStorage(globalTempDirs->root().absolutePath()));
}

void FullUpdateOperation::restartDevice()
{
    registerSubOperation(m_utility->restartDevice());
}
