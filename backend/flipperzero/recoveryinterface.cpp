#include "recoveryinterface.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/recovery/leaveoperation.h"
#include "flipperzero/recovery/fixbootissuesoperation.h"
#include "flipperzero/recovery/fixoptionbytesoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "recovery.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

RecoveryInterface::RecoveryInterface(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_recovery(new Recovery(state->deviceInfo().usbInfo, this))
{}

LeaveOperation *RecoveryInterface::exitRecoveryMode()
{
    auto *operation = new LeaveOperation(m_recovery, this);
    enqueueOperation(operation);
    return operation;
}

FirmwareDownloadOperation *RecoveryInterface::downloadFirmware(QIODevice *file)
{
    return nullptr;
}

WirelessStackDownloadOperation *RecoveryInterface::downloadFUS(QIODevice *file)
{
    return nullptr;
}

FixBootIssuesOperation *RecoveryInterface::fixBootIssues()
{
    return nullptr;
}

FixOptionBytesOperation *RecoveryInterface::fixOptionBytes(QIODevice *file)
{
    return nullptr;
}
