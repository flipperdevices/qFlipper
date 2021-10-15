#pragma once

#include "abstracttopleveloperation.h"

#include <QMap>
#include <QFile>

#include "flipperupdates.h"

namespace Flipper {
namespace Zero {

class RecoveryInterface;

class FullRepairOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        FetchingFirmware = AbstractOperation::User,
        FetchingCore2Firmware,
        PreparingRadioFirmware,
        FetchingScripts,
        PreparingOptionBytes,
        SettingBootMode,
        DownloadingRadioFirmware,
        DownloadingFirmware,
        CorrectingOptionBytes,
        CleaningUp
    };

    enum class FileIndex {
        Firmware,
        Core2Tgz,
        ScriptsTgz,
        RadioFirmware,
        OptionBytes
    };

    using FileDict = QMap<FileIndex, QFile*>;

public:
    FullRepairOperation(RecoveryInterface *recovery, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    ~FullRepairOperation();

    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void fetchFirmware();
    void fetchCore2Firmware();
    void prepareRadioFirmware();
    void fetchScripts();
    void prepareOptionBytes();
    void setBootMode();
    void downloadRadioFirmware();
    void downloadFirmware();
    void correctOptionBytes();
    void cleanupFiles();

    void fetchFile(FileIndex index, const Updates::FileInfo &fileInfo);

    RecoveryInterface *m_recovery;
    Updates::VersionInfo m_versionInfo;

    FileDict m_files;
};

}
}

