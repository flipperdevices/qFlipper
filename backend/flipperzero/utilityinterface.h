#pragma once

#include "abstractoperationrunner.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class DeviceState;
class ProtobufSession;

class FactoryResetUtilOperation;
class StartRecoveryOperation;
class AssetsDownloadOperation;
class UserBackupOperation;
class UserRestoreOperation;
class RestartOperation;

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

private:
    const QLoggingCategory &loggingCategory() const override;

    DeviceState *m_deviceState;
    ProtobufSession *m_rpc;
};

}
}

