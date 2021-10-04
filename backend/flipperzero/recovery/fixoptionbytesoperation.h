#pragma once

#include "abstractrecoveryoperation.h"
#include "flipperzero/flipperzerooperation.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class FixOptionBytesOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

public:
    FixOptionBytesOperation(Recovery *recovery, QIODevice *file, QObject *parent = nullptr);
    ~FixOptionBytesOperation();

    const QString description() const override;

private slots:
    void advanceOperationState() override;

private:
    void fixOptionBytes();
    QIODevice *m_file;
};

}
}
