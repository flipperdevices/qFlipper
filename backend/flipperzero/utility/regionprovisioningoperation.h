#pragma once

#include "abstractutilityoperation.h"

class QFile;

namespace Flipper {
namespace Zero {

class RegionProvisioningOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        FetchingRegionInfo = BasicOperationState::User,
        GeneratingRegionData,
        UploadingRegionData,
    };

public:
    RegionProvisioningOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent = nullptr);
    const QString description() const override;

private:
    void nextStateLogic() override;

    void fetchRegionInfo();
    void generateRegionData();
    void uploadRegionData();

    QFile *m_regionInfoFile;
    QFile* m_regionDataFile;
};

}
}

