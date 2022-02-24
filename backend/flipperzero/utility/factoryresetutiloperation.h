#pragma once

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class FactoryResetUtilOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        ResettingDevice = AbstractOperation::User
    };

public:
    FactoryResetUtilOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;
    void onDeviceOnlineChanged();

private:
    void resetDevice();
};

}
}

