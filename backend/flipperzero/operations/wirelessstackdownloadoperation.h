#pragma once

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class WirelessStackDownloadOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        WaitingForDFU = AbstractFirmwareOperation::User,
        WaitingForFirmwareBoot,
    };

public:
    WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress = 0, QObject *parent = nullptr);
    ~WirelessStackDownloadOperation();

    const QString name() const override;
    void start() override;

private slots:
    void transitionToNextState();
    void onOperationTimeout() override;

private:
    void doEnterDFUMode();

    FlipperZero *m_device;
    QIODevice *m_file;
    uint32_t m_targetAddress;
};

}
}
