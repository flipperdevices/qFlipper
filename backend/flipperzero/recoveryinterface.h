#pragma once

#include "abstractoperationrunner.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class Recovery;
class DeviceState;

class SetBootModeOperation;
class ExitRecoveryOperation;
class FirmwareDownloadOperation;
class FixBootIssuesOperation;
class CorrectOptionBytesOperation;
class WirelessStackDownloadOperation;

class RecoveryInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    RecoveryInterface(DeviceState *state, QObject *parent = nullptr);

    ExitRecoveryOperation *exitRecoveryMode();

    SetBootModeOperation *setOSBootMode();
    SetBootModeOperation *setRecoveryBootMode();

    FirmwareDownloadOperation *downloadFirmware(QIODevice *file);

    WirelessStackDownloadOperation *downloadFUS(QIODevice *file, uint32_t address);
    WirelessStackDownloadOperation *downloadWirelessStack(QIODevice *file);

    FixBootIssuesOperation *fixBootIssues();
    CorrectOptionBytesOperation *fixOptionBytes(QIODevice *file);

private:
    Recovery *m_recovery;
};

}
}

