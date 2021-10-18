#pragma once

#include "abstractoperationhelper.h"

#include <QMap>

#include "flipperupdates.h"

class QFile;

namespace Flipper {
namespace Zero {

class DeviceState;

class FirmwareHelper : public AbstractOperationHelper
{
    Q_OBJECT

    enum State {
        FetchingFirmware = AbstractOperationHelper::User,
        FetchingCore2Firmware,
        PreparingRadioFirmware,
        FetchingScripts,
        PreparingOptionBytes,
        FetchingAssets
    };

public:

    enum class FileIndex {
        Firmware,
        Core2Tgz,
        ScriptsTgz,
        AssetsTgz,
        RadioFirmware,
        OptionBytes
    };

    FirmwareHelper(DeviceState *deviceState, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    ~FirmwareHelper();

    QFile *file(FileIndex index) const;
    bool hasRadioUpdate() const;

private:
    void nextStateLogic() override;

    void fetchFirmware();
    void fetchCore2Firmware();
    void prepareRadioFirmware();
    void fetchScripts();
    void prepareOptionBytes();
    void fetchAssets();

    void fetchFile(FileIndex index, const Updates::FileInfo &fileInfo);

    DeviceState *m_deviceState;
    Updates::VersionInfo m_versionInfo;
    QMap<FileIndex, QFile*> m_files;
    bool m_hasRadioUpdate;
};

}
}

