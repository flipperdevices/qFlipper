#pragma once

#include "abstracttopleveloperation.h"

#include "flipperupdates.h"

namespace Flipper {
namespace Zero {

//class FirmwareHelper;
class UtilityInterface;

class InternalUpdateOperation : public Flipper::Zero::AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        FetchingFirmware = AbstractOperation::User,
        UploadingFimware,
        WaitingForUpdate
    };

public:
    InternalUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void fetchFirmware();
    void uploadFirmware();
    void startUpdate();

    void onSubOperationError(AbstractOperation *operation) override;

    UtilityInterface *m_utility;
//    FirmwareHelper *m_helper;

    Updates::VersionInfo m_versionInfo;
};

}
}

