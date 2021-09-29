#pragma once

#include "flipperzero/flipperzerooperation.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class FirmwareDownloadOperation : public FlipperZeroOperation
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

