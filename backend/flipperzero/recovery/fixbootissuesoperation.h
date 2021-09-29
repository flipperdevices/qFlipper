#pragma once

#include "flipperzero/flipperzerooperation.h"

namespace Flipper {
namespace Zero {

class FixBootIssuesOperation : public FlipperZeroOperation
{
    Q_OBJECT

    enum State {
        StartingWirelessStack = AbstractOperation::User,
        FixingBootMode
    };

public:
    FixBootIssuesOperation(FlipperZero *device, QObject *parent = nullptr);
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

