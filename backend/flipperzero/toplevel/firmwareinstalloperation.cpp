#include "firmwareinstalloperation.h"

#include <QFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"

#include "tempdirectories.h"

using namespace Flipper;
using namespace Zero;

FirmwareInstallOperation::FirmwareInstallOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_file(new QFile(filePath, this)),
    m_skipBackup(deviceState()->isRecoveryMode()),
    m_backupUrl(globalTempDirs->fileUrl(QStringLiteral("%1-%2.tgz").arg(state->deviceInfo().name, QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"))))
{}

const QString FirmwareInstallOperation::description() const
{
    return QStringLiteral("Firmware install from file @%1").arg(deviceState()->name());
}

void FirmwareInstallOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(FirmwareInstallOperation::SavingBackup);
        saveBackup();

    } else if(operationState() == FirmwareInstallOperation::SavingBackup) {
        setOperationState(FirmwareInstallOperation::StartingRecovery);
        startRecovery();

    } else if(operationState() == FirmwareInstallOperation::StartingRecovery) {
        setOperationState(FirmwareInstallOperation::InstallingFirmware);
        installFirmware();

    } else if(operationState() == FirmwareInstallOperation::InstallingFirmware) {
        setOperationState(FirmwareInstallOperation::ExitingRecovery);
        exitRecovery();

    } else if(operationState() == FirmwareInstallOperation::ExitingRecovery) {
        setOperationState(FirmwareInstallOperation::RestoringBackup);
        restoreBackup();

    } else if(operationState() == FirmwareInstallOperation::RestoringBackup) {
        setOperationState(FirmwareInstallOperation::RestartingDevice);
        restartDevice();

    } else if(operationState() == FirmwareInstallOperation::RestartingDevice) {
        finish();
    }
}

void FirmwareInstallOperation::saveBackup()
{
    if(m_skipBackup) {
        setOperationState(FirmwareInstallOperation::StartingRecovery);
        advanceOperationState();

    } else {
        registerSubOperation(m_utility->backupInternalStorage(m_backupUrl));
    }
}

void FirmwareInstallOperation::startRecovery()
{
    registerSubOperation(m_utility->startRecoveryMode());
}

void FirmwareInstallOperation::installFirmware()
{
    registerSubOperation(m_recovery->downloadFirmware(m_file));
}

void FirmwareInstallOperation::exitRecovery()
{
    registerSubOperation(m_recovery->exitRecoveryMode());
}

void FirmwareInstallOperation::restoreBackup()
{
    if(m_skipBackup) {
        finish();
    } else {
        registerSubOperation(m_utility->restoreInternalStorage(m_backupUrl));
    }
}

void FirmwareInstallOperation::restartDevice()
{
    registerSubOperation(m_utility->restartDevice());
}

void FirmwareInstallOperation::onSubOperationError(AbstractOperation *operation)
{
    const auto keepError = operationState() == FirmwareInstallOperation::SavingBackup ||
                           operationState() == FirmwareInstallOperation::StartingRecovery;

    finishWithError(keepError ? operation->error() : BackendError::OperationError, operation->errorString());
}
