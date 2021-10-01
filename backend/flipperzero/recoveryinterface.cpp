#include "recoveryinterface.h"

#include "recovery.h"

#include "flipperzero/recovery/fixbootissuesoperation.h"
#include "flipperzero/recovery/fixoptionbytesoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "remotefilefetcher.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

RecoveryInterface::RecoveryInterface(const USBDeviceInfo &deviceInfo, QObject *parent):
    AbstractOperationRunner(parent),
    m_recovery(new Recovery(deviceInfo, this))
{}

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
