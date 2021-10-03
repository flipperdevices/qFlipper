#pragma once

#include "recoveryoperation.h"

namespace Flipper {
namespace Zero {

class FixBootIssuesOperation : public RecoveryOperation
{
    Q_OBJECT

    enum State {
        StartingWirelessStack = AbstractOperation::User,
        FixingBootMode
    };

public:
    FixBootIssuesOperation(Recovery *recovery, QObject *parent = nullptr);
    ~FixBootIssuesOperation();

    const QString description() const override;

private slots:
    void transitionToNextState() override;
    void onOperationTimeout() override;

private:
    void startWirelessStack();
    void fixBootMode();
};

}
}

