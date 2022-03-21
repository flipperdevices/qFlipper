#include "recoveryinterface.h"

#include <QLoggingCategory>

#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/correctoptionbytesoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "recovery.h"
#include "debug.h"

Q_LOGGING_CATEGORY(LOG_RECOVERY, "RCY")

using namespace Flipper;
using namespace Zero;

RecoveryInterface::RecoveryInterface(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_recovery(new Recovery(state, this))
{}

ExitRecoveryOperation *RecoveryInterface::exitRecoveryMode()
{
    auto *operation = new ExitRecoveryOperation(m_recovery, this);
    enqueueOperation(operation);
    return operation;
}

SetBootModeOperation *RecoveryInterface::setOSBootMode()
{
    auto *operation = new SetOSBootOperation(m_recovery, this);
    enqueueOperation(operation);
    return operation;
}

SetBootModeOperation *RecoveryInterface::setRecoveryBootMode()
{
    auto *operation = new SetRecoveryBootOperation(m_recovery, this);
    enqueueOperation(operation);
    return operation;
}

FirmwareDownloadOperation *RecoveryInterface::downloadFirmware(QIODevice *file)
{
    auto *operation = new FirmwareDownloadOperation(m_recovery, file, this);
    enqueueOperation(operation);
    return operation;
}

WirelessStackDownloadOperation *RecoveryInterface::downloadFUS(QIODevice *file, uint32_t address)
{
    auto *operation = new WirelessStackDownloadOperation(m_recovery, file, address, this);
    enqueueOperation(operation);
    return operation;
}

WirelessStackDownloadOperation *RecoveryInterface::downloadWirelessStack(QIODevice *file)
{
    auto *operation = new WirelessStackDownloadOperation(m_recovery, file, 0, this);
    enqueueOperation(operation);
    return operation;
}

CorrectOptionBytesOperation *RecoveryInterface::fixOptionBytes(QIODevice *file)
{
    auto *operation = new CorrectOptionBytesOperation(m_recovery, file, this);
    enqueueOperation(operation);
    return operation;
}

const QLoggingCategory &RecoveryInterface::loggingCategory() const
{
    return LOG_RECOVERY();
}
