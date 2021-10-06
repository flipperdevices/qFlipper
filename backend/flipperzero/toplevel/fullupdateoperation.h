#pragma once

#include "abstracttopleveloperation.h"

#include <QDir>

#include "flipperupdates.h"

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;
class RecoveryInterface;

class FullUpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        CreatingWorkDir = AbstractOperation::User,
        FetchingFirmware,
        FetchingAssets,
        SavingBackup,
        StartingRecovery,
        DownloadingFirmware,
        ExitingRecovery,
        DownloadingAssets,
        RestoringBackup,
        RestartingDevice,
        CleaningUp
    };

public:
    FullUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    ~FullUpdateOperation();

    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void onSubOperationErrorOccured() override;

    void createWorkDir();
    void fetchFirmware();
    void fetchAssets();
    void saveBackup();
    void startRecovery();
    void downloadFirmware();
    void exitRecovery();
    void downloadAssets();
    void restoreBackup();
    void restartDevice();
    void cleanupFiles();

    QFile *fetchFile(const Updates::FileInfo &fileInfo);

    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
    Updates::VersionInfo m_versionInfo;

    QDir m_workDir;
    QFile *m_firmwareFile;
    QFile *m_assetsFile;
};

}
}

