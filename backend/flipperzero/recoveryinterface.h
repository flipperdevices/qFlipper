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

    CorrectOptionBytesOperation *fixOptionBytes(QIODevice *file);

private:
    const QLoggingCategory &loggingCategory() const override;

    Recovery *m_recovery;
};

}
}

