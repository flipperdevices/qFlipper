#pragma once

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FirmwareUpdateOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        WaitingForDFU = AbstractFirmwareOperation::User,
        DownloadingFirmware,
        WaitingForFirmwareBoot,
    };

public:
    FirmwareUpdateOperation(FlipperZero *device, QIODevice *file, QObject *parent = nullptr);
    ~FirmwareUpdateOperation();

    const QString name() const override;
    void start() override;

private slots:
    void transitionToNextState();
    void onOperationTimeout() override;

private:
    void doEnterDFUMode();
    void doDownloadFirmware();
    void doBootFirmware();

    FlipperZero *m_device;
    QIODevice *m_file;
};

}
}

