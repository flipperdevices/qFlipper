#pragma once

#include "flipperzerooperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FixOptionBytesOperation : public Operation
{
    Q_OBJECT

    enum State {
        BootingToDFU = AbstractOperation::User,
        FixingOptionBytes,
    };

public:
    FixOptionBytesOperation(FlipperZero *device, QIODevice *file, QObject *parent = nullptr);
    ~FixOptionBytesOperation();

    const QString description() const override;

private slots:
    void transitionToNextState() override;
    void onOperationTimeout() override;

private:
    void bootToDFU();
    void fixOptionBytes();

    QIODevice *m_file;
};

}
}
