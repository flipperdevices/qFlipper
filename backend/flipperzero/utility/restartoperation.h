#pragma once

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class RestartOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        WaitingForOSBoot = AbstractOperation::User
    };

public:
    RestartOperation(CommandInterface *cli, DeviceState *deviceState, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void advanceOperationState() override;
    void onDeviceOnlineChanged();

private:
    void rebootDevice();
};

}
}

