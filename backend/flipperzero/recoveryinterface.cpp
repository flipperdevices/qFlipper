#include "recoveryinterface.h"

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
    m_recovery(new Recovery(state, this))
{}

LeaveOperation *RecoveryInterface::exitRecoveryMode()
{
    auto *operation = new LeaveOperation(m_recovery, this);
    enqueueOperation(operation);
    return operation;
}

FirmwareDownloadOperation *RecoveryInterface::downloadFirmware(QIODevice *file)
{
    auto *operation = new FirmwareDownloadOperation(m_recovery, file, this);
    enqueueOperation(operation);
    return operation;
}

WirelessStackDownloadOperation *RecoveryInterface::downloadFUS(QIODevice *file)
{
    // TODO: allow for different FUS addresses
    auto *operation = new WirelessStackDownloadOperation(m_recovery, file, 0x080ec000, this);
    enqueueOperation(operation);
    return operation;
}

WirelessStackDownloadOperation *RecoveryInterface::downloadWirelessStack(QIODevice *file)
{
    auto *operation = new WirelessStackDownloadOperation(m_recovery, file, 0, this);
    enqueueOperation(operation);
    return operation;
}

FixBootIssuesOperation *RecoveryInterface::fixBootIssues()
{
    auto *operation = new FixBootIssuesOperation(m_recovery, this);
    enqueueOperation(operation);
    return operation;
}

FixOptionBytesOperation *RecoveryInterface::fixOptionBytes(QIODevice *file)
{
    auto *operation = new FixOptionBytesOperation(m_recovery, file, this);
    enqueueOperation(operation);
    return operation;
}
