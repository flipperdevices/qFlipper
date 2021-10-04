#pragma once

#include "abstractoperation.h"

namespace Flipper {
namespace Zero {

class DeviceState;

class AbstractTopLevelOperation : public AbstractOperation
{
    Q_OBJECT

public:
    AbstractTopLevelOperation(DeviceState *deviceState, QObject *parent = nullptr);
    virtual ~AbstractTopLevelOperation() {}

    DeviceState *deviceState() const;

    void start() override;

private slots:
    virtual void advanceOperationState() = 0;

private:
    DeviceState *m_deviceState;
};

}
}

