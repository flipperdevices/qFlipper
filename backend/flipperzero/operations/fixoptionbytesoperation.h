#pragma once

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FixOptionBytesOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        WaitingForDFU = AbstractFirmwareOperation::User,
        WaitingForFirmwareBoot,
    };

public:
    FixOptionBytesOperation(FlipperZero *device, QIODevice *file, QObject *parent = nullptr);
    ~FixOptionBytesOperation();

    const QString name() const override;
    void start() override;

private slots:
    void transitionToNextState();
    void onOperationTimeout() override;

private:
    void doEnterDFUMode();
    void doFixOptionBytes();

    FlipperZero *m_device;
    QIODevice *m_file;
};

}
}
