#pragma once

#include "flipperzerooperation.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class FixBootIssuesOperation : public Operation
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

