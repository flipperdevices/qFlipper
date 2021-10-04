#pragma once

#include "abstractrecoveryoperation.h"

namespace Flipper {
namespace Zero {

class LeaveOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

    enum OperationState {
        WaitingForOnline = BasicOperationState::User
    };

public:
    LeaveOperation(Recovery *recovery, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void advanceOperationState() override;
};

}
}

