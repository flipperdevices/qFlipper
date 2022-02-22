#pragma once

#include "abstractoperation.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class ProtobufSession;

class AbstractUtilityOperation : public AbstractOperation
{
    Q_OBJECT

public:
    AbstractUtilityOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent = nullptr);
    virtual ~AbstractUtilityOperation() {}

    void start() override;

    ProtobufSession *rpc() const;
    DeviceState *deviceState() const;

protected:
    void advanceOperationState();

private slots:
    virtual void nextStateLogic() = 0;

private:
    ProtobufSession *m_rpc;
    DeviceState *m_deviceState;
};

}
}

