#pragma once

#include "abstractoperation.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class FlipperZeroOperation : public AbstractOperation
{
    Q_OBJECT

public:
    FlipperZeroOperation(FlipperZero *device, QObject *parent = nullptr);
    virtual ~FlipperZeroOperation();

    FlipperZero *device() const;

    void start() override;
    void finish() override;

private slots:
    virtual void transitionToNextState() = 0;

private:
    FlipperZero *m_device;
};

}
}
