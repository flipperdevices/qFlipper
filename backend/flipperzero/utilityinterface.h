#pragma once

#include "abstractoperationrunner.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class DeviceState;
class ProtobufSession;

class DirectoryUploadOperation;
class DirectoryDownloadOperation;
class FactoryResetUtilOperation;
class StartRecoveryOperation;
class AssetsDownloadOperation;
class UserBackupOperation;
class UserRestoreOperation;
class RestartOperation;
class UpdatePrepareOperation;
class StartUpdaterOperation;
class SDCardCheckOperation;

class UtilityInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    UtilityInterface(DeviceState *deviceState, ProtobufSession *rpc, QObject *parent = nullptr);

    StartRecoveryOperation *startRecoveryMode();
    AssetsDownloadOperation *downloadAssets(QIODevice *compressedFile);
    UserBackupOperation *backupInternalStorage(const QString &backupPath);
    UserRestoreOperation *restoreInternalStorage(const QString &backupPath);
    RestartOperation *restartDevice();
    FactoryResetUtilOperation *factoryReset();
    DirectoryUploadOperation *uploadDirectory(const QString &localDirectory, const QByteArray &remotePath);
    DirectoryDownloadOperation *downloadDirectory(const QString &localDirectory, const QByteArray &remotePath);
    UpdatePrepareOperation *prepareUpdateDirectory(const QByteArray &updateDirName, const QByteArray &remotePath);
    StartUpdaterOperation *startUpdater(const QByteArray &manifestPath);
    SDCardCheckOperation *checkStorage();

private:
    const QLoggingCategory &loggingCategory() const override;

    DeviceState *m_deviceState;
    ProtobufSession *m_rpc;
};

}
}

