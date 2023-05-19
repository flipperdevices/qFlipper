#pragma once

#include "abstractoperationrunner.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class DeviceState;
class ProtobufSession;

class FilesUploadOperation;
class DirectoryDownloadOperation;
class FactoryResetUtilOperation;
class StartRecoveryOperation;
class AssetsDownloadOperation;
class UserBackupOperation;
class UserRestoreOperation;
class RestartOperation;
class PathCreateOperation;
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
    DirectoryDownloadOperation *downloadDirectory(const QString &localDirectory, const QByteArray &remotePath);
    PathCreateOperation *createPath(const QByteArray &remotePath);
    StartUpdaterOperation *startUpdater(const QByteArray &manifestPath);
    StorageInfoRefreshOperation *refreshStorageInfo();
    RegionProvisioningOperation *provisionRegionData();
    ChecksumVerifyOperation *verifyChecksum(const QList<QUrl> &urlsToCheck, const QByteArray &remoteRootPath);

private:
    const QLoggingCategory &loggingCategory() const override;

    DeviceState *m_deviceState;
    ProtobufSession *m_rpc;
};

}
}

