#pragma once

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FirmwareDownloadOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        WaitingForDFU = AbstractFirmwareOperation::User,
        DownloadingFirmware,
        WaitingForFirmwareBoot,
    };

public:
    FirmwareDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent = nullptr);
    ~FirmwareDownloadOperation();

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

