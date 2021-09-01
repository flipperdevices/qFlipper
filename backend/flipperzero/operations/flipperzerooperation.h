#pragma once

#include "abstractoperation.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class Operation : public AbstractOperation
{
    Q_OBJECT

public:
    Operation(FlipperZero *device, QObject *parent = nullptr);
    virtual ~Operation();

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
