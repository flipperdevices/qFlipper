#include "settingsrestoreoperation.h"

#include <QTimer>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"

using namespace Flipper;
using namespace Zero;

static constexpr qint64 MINIMUM_OPERATION_TIME_MS = 2000;

SettingsRestoreOperation::SettingsRestoreOperation(UtilityInterface *utility, DeviceState *state, const QUrl &backupUrl, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_utility(utility),
    m_backupUrl(backupUrl)
{}

const QString SettingsRestoreOperation::description() const
{
    return QStringLiteral("Restore Internal storage @%1").arg(deviceState()->name());
}

void SettingsRestoreOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(SettingsRestoreOperation::RestoringBackup);
        restoreBackup();

    } else if(operationState() == SettingsRestoreOperation::RestoringBackup) {
        setOperationState(SettingsRestoreOperation::Waiting);
        wait();

    } else if(operationState() == SettingsRestoreOperation::Waiting) {
        setOperationState(SettingsRestoreOperation::RestartingDevice);
        restartDevice();

    } else if(operationState() == SettingsRestoreOperation::RestartingDevice) {
        finish();
    }
}

void SettingsRestoreOperation::restoreBackup()
{
    m_elapsed.start();
    registerSubOperation(m_utility->restoreInternalStorage(m_backupUrl));
}

void SettingsRestoreOperation::wait()
{
    const auto delay = qMax<qint64>(MINIMUM_OPERATION_TIME_MS - m_elapsed.elapsed(), 0);
    QTimer::singleShot(delay, this, &SettingsRestoreOperation::advanceOperationState);
}

void SettingsRestoreOperation::restartDevice()
{
    registerSubOperation(m_utility->restartDevice());
}

void SettingsRestoreOperation::onSubOperationError(AbstractOperation *operation)
{
    const auto keepError = operationState() == SettingsRestoreOperation::RestoringBackup;
    finishWithError(keepError ? operation->error() : BackendError::OperationError, operation->errorString());
}
