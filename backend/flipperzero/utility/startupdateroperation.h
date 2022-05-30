#pragma once

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class StartUpdaterOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        StartingUpdate = AbstractOperation::User,
        RebootingToUpdater,
        WaitingForUpdate
    };

public:
    StartUpdaterOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &manifestPath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onOperationTimeout() override;
    void onDeviceOnlineChanged();

private:
    void nextStateLogic() override;

    void startUpdate();
    void rebootToUpdater();
    void waitForUpdate();

    QByteArray m_manifestPath;
};

}
}

