#pragma once

#include "abstractfirmwareoperation.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class FixBootIssuesOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        WaitingForWirelessStack = AbstractFirmwareOperation::User,
        WaitingForFirmwareBoot
    };

public:
    FixBootIssuesOperation(FlipperZero *device, QObject *parent = nullptr);
    ~FixBootIssuesOperation();

    const QString name() const override;
    void start() override;

private slots:
    void transitionToNextState();
    void onOperationTimeout() override;

private:
    void doStartWirelessStack();
    void doFixBootMode();

    FlipperZero *m_device;
};

}
}

