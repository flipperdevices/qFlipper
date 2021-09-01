#pragma once

#include "abstractfirmwareoperation.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class FixBootIssuesOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        StartingWirelessStack = AbstractFirmwareOperation::User,
        FixingBootMode
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
    void startWirelessStack();
    void fixBootMode();
    void finish();

    FlipperZero *m_device;
};

}
}

