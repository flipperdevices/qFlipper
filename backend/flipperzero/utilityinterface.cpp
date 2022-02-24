#include "utilityinterface.h"

#include <QLoggingCategory>

#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"
#include "flipperzero/utility/factoryresetutiloperation.h"

Q_LOGGING_CATEGORY(CATEGORY_UTILITY, "UTILITY")

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

const QLoggingCategory &UtilityInterface::loggingCategory() const
{
    return CATEGORY_UTILITY();
}
