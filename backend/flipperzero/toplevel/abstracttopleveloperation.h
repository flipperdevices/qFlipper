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
    virtual ~AbstractTopLevelOperation();

    DeviceState *deviceState() const;

    void start() override;

protected:
    void advanceOperationState();
    void registerSubOperation(AbstractOperation *operation);
    virtual void onSubOperationError(AbstractOperation *operation);

private slots:
    virtual void nextStateLogic() = 0;

private:
    DeviceState *m_deviceState;
};

}
}

