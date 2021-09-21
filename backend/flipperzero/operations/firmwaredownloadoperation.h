#pragma once

#include "flipperzerooperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FirmwareDownloadOperation : public Operation
{
    Q_OBJECT

    enum State {
        BootingToDFU = AbstractOperation::User,
        DownloadingFirmware,
        BootingToFirmware,
    };

public:
    FirmwareDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent = nullptr);
    ~FirmwareDownloadOperation();

    const QString description() const override;

private slots:
    void transitionToNextState() override;
    void onOperationTimeout() override;

private:
    void booToDFU();
    void downloadFirmware();
    void bootToFirmware();

    QIODevice *m_file;
};

}
}

