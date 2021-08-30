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
        WaitForDFU = AbstractFirmwareOperation::User,
        Downloading,
        WaitForVCP,
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
    FlipperZero *m_device;
    QIODevice *m_file;
};

}
}

