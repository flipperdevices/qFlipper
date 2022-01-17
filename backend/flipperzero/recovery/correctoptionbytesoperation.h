#pragma once

#include "abstractrecoveryoperation.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class CorrectOptionBytesOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

    enum OperationState {
        CorrectingOptionBytes = AbstractOperation::User
    };

public:
    CorrectOptionBytesOperation(Recovery *recovery, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;
    void onOperationTimeout() override;

private:
    void correctOptionBytes();
    QIODevice *m_file;
};

}
}
