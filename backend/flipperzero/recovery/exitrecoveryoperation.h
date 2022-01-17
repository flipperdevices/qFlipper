#pragma once

#include "abstractrecoveryoperation.h"

namespace Flipper {
namespace Zero {

class ExitRecoveryOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

    enum OperationState {
        WaitingForOnline = BasicOperationState::User
    };

public:
    ExitRecoveryOperation(Recovery *recovery, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;
    void onOperationTimeout() override;

private:
    void exitRecovery();
};

}
}

