#include "firmwareupdateoperation.h"

#include <QFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/startrecoveryoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"

using namespace Flipper;
using namespace Zero;

FirmwareUpdateOperation::FirmwareUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_file(new QFile(filePath, this))
{}

const QString FirmwareUpdateOperation::description() const
{
    return QStringLiteral("Firmware Update @%1").arg(deviceState()->name());
}

void FirmwareUpdateOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(FirmwareUpdateOperation::StartingRecovery);
        startRecoveryMode();

    } else if(operationState() == FirmwareUpdateOperation::StartingRecovery) {
        setOperationState(FirmwareUpdateOperation::UpdatingFirmware);
        updateFirmware();

    } else if(operationState() == FirmwareUpdateOperation::UpdatingFirmware) {
        setOperationState(FirmwareUpdateOperation::ExitingRecovery);
        exitRecoveryMode();

    } else if(operationState() == FirmwareUpdateOperation::ExitingRecovery) {
        finish();
    }
}

void FirmwareUpdateOperation::startRecoveryMode()
{
    registerOperation(m_utility->startRecoveryMode());
}

void FirmwareUpdateOperation::updateFirmware()
{
    registerOperation(m_recovery->downloadFirmware(m_file));
}

void FirmwareUpdateOperation::exitRecoveryMode()
{
    registerOperation(m_recovery->exitRecoveryMode());
}
