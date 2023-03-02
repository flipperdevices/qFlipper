#pragma once

#include "abstracttopleveloperation.h"
#include "flipperupdates.h"

namespace Flipper {
namespace Zero {

class FirmwareHelper;
class UtilityInterface;
class RecoveryInterface;

class FullRepairOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        FetchingFirmware = AbstractOperation::User,
        SettingBootMode,
        DownloadingRadioFirmware,
        DownloadingFirmware,
        CorrectingOptionBytes,
        DownloadingAssets,
        ProvisioningRegion,
        RestartingDevice
    };

public:
    FullRepairOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void fetchFirmware();
    void setBootMode();
    void downloadRadioFirmware();
    void downloadFirmware();
    void correctOptionBytes();
    void downloadAssets();
    void provisionRegion();
    void restartDevice();

    void onSubOperationError(AbstractOperation *operation) override;

    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
    FirmwareHelper *m_helper;

    Updates::VersionInfo m_versionInfo;
};

}
}

