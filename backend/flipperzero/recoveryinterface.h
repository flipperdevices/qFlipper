#pragma once

#include "abstractoperationrunner.h"

#include "usbdeviceinfo.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class Recovery;

class FirmwareDownloadOperation;
class FixBootIssuesOperation;
class FixOptionBytesOperation;
class WirelessStackDownloadOperation;

class RecoveryInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    RecoveryInterface(const USBDeviceInfo &deviceInfo, QObject *parent = nullptr);

    FirmwareDownloadOperation *downloadFirmware(QIODevice *file);

    WirelessStackDownloadOperation *downloadFUS(QIODevice *file);
    WirelessStackDownloadOperation *downloadWirelessStack(QIODevice *file);

    FixBootIssuesOperation *fixBootIssues();
    FixOptionBytesOperation *fixOptionBytes(QIODevice *file);

private:
    Recovery *m_recovery;
};

}
}

