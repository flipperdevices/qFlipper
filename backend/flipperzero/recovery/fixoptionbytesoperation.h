#pragma once

#include "flipperzero/flipperzerooperation.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class FixOptionBytesOperation : public FlipperZeroOperation
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
