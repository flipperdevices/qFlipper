#pragma once

#include "abstractoperationrunner.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class DeviceState;
class ProtobufSession;

class FilesUploadOperation;
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
class StorageInfoRefreshOperation;
class RegionProvisioningOperation;
class ChecksumVerifyOperation;

class UtilityInterface : public AbstractOperationRunner
{
    Q_OBJECT

public:
    UtilityInterface(DeviceState *deviceState, ProtobufSession *rpc, QObject *parent = nullptr);

    StartRecoveryOperation *startRecoveryMode();
    AssetsDownloadOperation *downloadAssets(QIODevice *compressedFile);
    UserBackupOperation *backupInternalStorage(const QUrl &backupUrl);
    UserRestoreOperation *restoreInternalStorage(const QUrl &backupUrl);
    RestartOperation *restartDevice();
    FactoryResetUtilOperation *factoryReset();
    FilesUploadOperation *uploadFiles(const QList<QUrl> &fileUrls, const QByteArray &remotePath);
    DirectoryUploadOperation *uploadDirectory(const QString &localDirectory, const QByteArray &remotePath);
    DirectoryDownloadOperation *downloadDirectory(const QString &localDirectory, const QByteArray &remotePath);
    UpdatePrepareOperation *prepareUpdateDirectory(const QByteArray &updateDirName, const QByteArray &remotePath);
    StartUpdaterOperation *startUpdater(const QByteArray &manifestPath);
    StorageInfoRefreshOperation *refreshStorageInfo();
    RegionProvisioningOperation *provisionRegionData();
    ChecksumVerifyOperation *verifyChecksum(const QString &localDirectory, const QByteArray &remotePath);

private:
    const QLoggingCategory &loggingCategory() const override;

    DeviceState *m_deviceState;
    ProtobufSession *m_rpc;
};

}
}

