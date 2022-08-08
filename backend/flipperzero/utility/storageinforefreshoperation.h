#pragma once

#include "abstractutilityoperation.h"
#include "flipperzero/deviceinfo.h"

namespace Flipper {
namespace Zero {

class StorageInfoRefreshOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        CheckingSDCard = AbstractOperation::User,
        CheckingManifest
    };

public:
    StorageInfoRefreshOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void checkSDCard();
    void checkManifest();
    void updateStorageInfo();

    StorageInfo m_storageInfo;
};

}
}

