#include "utilityinterface.h"

#include "devicestate.h"
#include "commandinterface.h"

#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"

using namespace Flipper;
using namespace Zero;

UtilityInterface::UtilityInterface(DeviceState *deviceState, QObject *parent):
    AbstractOperationRunner(parent),
    m_deviceState(deviceState),
    m_cli(new CommandInterface(deviceState, this))
{}

StartRecoveryOperation *UtilityInterface::startRecoveryMode()
{
    auto *operation = new StartRecoveryOperation(m_cli, m_deviceState, this);
    enqueueOperation(operation);
    return operation;
}

AssetsDownloadOperation *UtilityInterface::downloadAssets(QIODevice *compressedFile)
{
    auto *operation = new AssetsDownloadOperation(m_cli, m_deviceState, compressedFile, this);
    enqueueOperation(operation);
    return operation;
}

UserBackupOperation *UtilityInterface::backupInternalStorage(const QString &backupPath)
{
    auto *operation = new UserBackupOperation(m_cli, m_deviceState, backupPath, this);
    enqueueOperation(operation);
    return operation;
}

UserRestoreOperation *UtilityInterface::restoreInternalStorage(const QString &backupPath)
{
    auto *operation = new UserRestoreOperation(m_cli, m_deviceState, backupPath, this);
    enqueueOperation(operation);
    return operation;
}

RestartOperation *UtilityInterface::restartDevice()
{
    auto *operation = new RestartOperation(m_cli, m_deviceState, this);
    enqueueOperation(operation);
    return operation;
}
