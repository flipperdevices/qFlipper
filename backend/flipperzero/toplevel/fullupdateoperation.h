#pragma once

#include "abstracttopleveloperation.h"

#include <QMap>
#include <QFile>
#include <QSharedPointer>

#include "flipperupdates.h"

class QBuffer;

namespace Flipper {
namespace Zero {

class UtilityInterface;
class RecoveryInterface;

class FullUpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        FetchingFirmware = AbstractOperation::User,
        FetchingCore2Firmware,
        PreparingRadioFirmware,
        FetchingScripts,
        PreparingOptionBytes,
        FetchingAssets,
        SavingBackup,
        StartingRecovery,
        SettingBootMode,
        DownloadingRadioFirmware,
        DownloadingFirmware,
        CorrectingOptionBytes,
        ExitingRecovery,
        DownloadingAssets,
        RestoringBackup,
        RestartingDevice,
        CleaningUp
    };

    enum class FileIndex {
        Firmware,
        Core2Tgz,
        ScriptsTgz,
        AssetsTgz,
        RadioFirmware,
        OptionBytes
    };

    using FileDict = QMap<FileIndex, QFile*>;

public:
    FullUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    ~FullUpdateOperation();

    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void onSubOperationErrorOccured() override;

    void fetchFirmware();
    void fetchCore2Firmware();
    void prepareRadioFirmware();
    void fetchScripts();
    void prepareOptionBytes();
    void fetchAssets();
    void saveBackup();
    void startRecovery();
    void setBootMode();
    void downloadRadioFirmware();
    void downloadFirmware();
    void correctOptionBytes();
    void exitRecovery();
    void downloadAssets();
    void restoreBackup();
    void restartDevice();
    void cleanupFiles();

    void fetchFile(FileIndex index, const Updates::FileInfo &fileInfo);

    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
    Updates::VersionInfo m_versionInfo;

    FileDict m_files;
    bool m_updateRadio;
    QBuffer *m_radioBuffer;
    QBuffer *m_optionBuffer;
};

}
}

