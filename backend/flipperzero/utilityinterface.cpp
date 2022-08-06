#include "utilityinterface.h"

#include <QLoggingCategory>

#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"
#include "flipperzero/utility/factoryresetutiloperation.h"
#include "flipperzero/utility/directoryuploadoperation.h"
#include "flipperzero/utility/directorydownloadoperation.h"
#include "flipperzero/utility/updateprepareoperation.h"
#include "flipperzero/utility/startupdateroperation.h"
#include "flipperzero/utility/sdcardcheckoperation.h"

Q_LOGGING_CATEGORY(LOG_UTILITY, "UTL")

using namespace Flipper;
using namespace Zero;

UtilityInterface::UtilityInterface(DeviceState *deviceState, ProtobufSession *rpc, QObject *parent):
    AbstractOperationRunner(parent),
    m_deviceState(deviceState),
    m_rpc(rpc)
{}

StartRecoveryOperation *UtilityInterface::startRecoveryMode()
{
    auto *operation = new StartRecoveryOperation(m_rpc, m_deviceState, this);
    enqueueOperation(operation);
    return operation;
}

AssetsDownloadOperation *UtilityInterface::downloadAssets(QIODevice *compressedFile)
{
    auto *operation = new AssetsDownloadOperation(m_rpc, m_deviceState, compressedFile, this);
    enqueueOperation(operation);
    return operation;
}

UserBackupOperation *UtilityInterface::backupInternalStorage(const QString &backupPath)
{
    auto *operation = new UserBackupOperation(m_rpc, m_deviceState, backupPath, this);
    enqueueOperation(operation);
    return operation;
}

UserRestoreOperation *UtilityInterface::restoreInternalStorage(const QString &backupPath)
{
    auto *operation = new UserRestoreOperation(m_rpc, m_deviceState, backupPath, this);
    enqueueOperation(operation);
    return operation;
}

RestartOperation *UtilityInterface::restartDevice()
{
    auto *operation = new RestartOperation(m_rpc, m_deviceState, this);
    enqueueOperation(operation);
    return operation;
}

FactoryResetUtilOperation *UtilityInterface::factoryReset()
{
    auto *operation = new FactoryResetUtilOperation(m_rpc, m_deviceState, this);
    enqueueOperation(operation);
    return operation;
}

DirectoryUploadOperation *UtilityInterface::uploadDirectory(const QString &localDirectory, const QByteArray &remotePath)
{
    auto *operation = new DirectoryUploadOperation(m_rpc, m_deviceState, localDirectory, remotePath, this);
    enqueueOperation(operation);
    return operation;
}

DirectoryDownloadOperation *UtilityInterface::downloadDirectory(const QString &localDirectory, const QByteArray &remotePath)
{
    auto *operation = new DirectoryDownloadOperation(m_rpc, m_deviceState, localDirectory, remotePath, this);
    enqueueOperation(operation);
    return operation;
}

UpdatePrepareOperation *UtilityInterface::prepareUpdateDirectory(const QByteArray &updateDirName, const QByteArray &remotePath)
{
    auto *operation = new UpdatePrepareOperation(m_rpc, m_deviceState, updateDirName, remotePath, this);
    enqueueOperation(operation);
    return operation;
}

StartUpdaterOperation *UtilityInterface::startUpdater(const QByteArray &manifestPath)
{
    auto *operation = new StartUpdaterOperation(m_rpc, m_deviceState, manifestPath, this);
    enqueueOperation(operation);
    return operation;
}

SDCardCheckOperation *UtilityInterface::checkStorage()
{
    auto *operation = new SDCardCheckOperation(m_rpc, m_deviceState, this);
    enqueueOperation(operation);
    return operation;
}

const QLoggingCategory &UtilityInterface::loggingCategory() const
{
    return LOG_UTILITY();
}
