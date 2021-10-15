#pragma once

#include "abstractrecoveryoperation.h"

namespace Flipper {
namespace Zero {

class FixBootIssuesOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

    enum State {
        StartingWirelessStack = AbstractOperation::User,
        FixingBootMode
    };

public:
    FixBootIssuesOperation(Recovery *recovery, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void advanceOperationState() override;
    void onOperationTimeout() override;

private:
    void startWirelessStack();
    void fixBootMode();
};

}
}

