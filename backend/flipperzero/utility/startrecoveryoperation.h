#pragma once

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class StartRecoveryOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        WaitingForRecovery = AbstractOperation::User
    };

public:
    StartRecoveryOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;
    void onOperationTimeout() override;
    void onDeviceOnlineChanged();

private:
    void startRecoveryMode();
};

}
}

