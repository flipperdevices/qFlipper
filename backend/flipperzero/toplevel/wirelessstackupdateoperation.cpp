#include "wirelessstackupdateoperation.h"

#include <QFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/startrecoveryoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

using namespace Flipper;
using namespace Zero;

AbstractCore2UpdateOperation::AbstractCore2UpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_file(new QFile(filePath, this))
{}

void AbstractCore2UpdateOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(AbstractCore2UpdateOperation::StartingRecovery);
        startRecoveryMode();

    } else if(operationState() == AbstractCore2UpdateOperation::StartingRecovery) {
        setOperationState(AbstractCore2UpdateOperation::SettingRecoveryBootMode);
        setRecoveryBootMode();

    } else if(operationState() == AbstractCore2UpdateOperation::SettingRecoveryBootMode) {
        setOperationState(AbstractCore2UpdateOperation::UpdatingWirelessStack);
        updateCore2Firmware();

    } else if(operationState() == AbstractCore2UpdateOperation::UpdatingWirelessStack) {
        setOperationState(AbstractCore2UpdateOperation::SettingOSBootMode);
        setOSBootMode();

    } else if(operationState() == AbstractCore2UpdateOperation::SettingOSBootMode) {
        finish();
    }
}

void AbstractCore2UpdateOperation::startRecoveryMode()
{
    registerSubOperation(m_utility->startRecoveryMode());
}

void AbstractCore2UpdateOperation::setRecoveryBootMode()
{
    registerSubOperation(m_recovery->setRecoveryBootMode());
}

void AbstractCore2UpdateOperation::setOSBootMode()
{
    registerSubOperation(m_recovery->setOSBootMode());
}

void AbstractCore2UpdateOperation::onSubOperationError(AbstractOperation *operation)
{
    const auto keepError = operationState() == AbstractCore2UpdateOperation::StartingRecovery;
    finishWithError(keepError ? operation->error() : BackendError::OperationError, operation->errorString());
}

WirelessStackUpdateOperation::WirelessStackUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent):
    AbstractCore2UpdateOperation(recovery, utility, state, filePath, parent)
{}

const QString WirelessStackUpdateOperation::description() const
{
    return QStringLiteral("Wireless Stack Update @%1").arg(deviceState()->name());
}

void WirelessStackUpdateOperation::updateCore2Firmware()
{
    registerSubOperation(m_recovery->downloadWirelessStack(m_file));
}

FUSUpdateOperation::FUSUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, uint32_t address, QObject *parent):
    AbstractCore2UpdateOperation(recovery, utility, state, filePath, parent),
    m_address(address)
{}

const QString FUSUpdateOperation::description() const
{
    return QStringLiteral("FUS Update @%1").arg(deviceState()->name());
}

void FUSUpdateOperation::updateCore2Firmware()
{
    registerSubOperation(m_recovery->downloadFUS(m_file, m_address));
}
