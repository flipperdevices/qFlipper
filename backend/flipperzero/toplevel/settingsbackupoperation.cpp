#include "settingsbackupoperation.h"

#include <QUrl>
#include <QTimer>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/userbackupoperation.h"

using namespace Flipper;
using namespace Zero;

static constexpr qint64 MINIMUM_OPERATION_TIME_MS = 2000;

SettingsBackupOperation::SettingsBackupOperation(UtilityInterface *utility, DeviceState *state, const QUrl &backupUrl, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_utility(utility),
    m_backupUrl(backupUrl)
{}

const QString SettingsBackupOperation::description() const
{
    return QStringLiteral("Backup Internal storage @%1").arg(deviceState()->name());
}

void SettingsBackupOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(SettingsBackupOperation::SavingBackup);
        saveBackup();

    } else if(operationState() == SettingsBackupOperation::SavingBackup) {
        setOperationState(SettingsBackupOperation::Waiting);
        wait();

    } else if(operationState() == SettingsBackupOperation::Waiting) {
        finish();
    }
}

void SettingsBackupOperation::saveBackup()
{
    m_elapsed.start();
    registerSubOperation(m_utility->backupInternalStorage(m_backupUrl));
}

void SettingsBackupOperation::wait()
{
    const auto delay = qMax<qint64>(MINIMUM_OPERATION_TIME_MS - m_elapsed.elapsed(), 0);
    QTimer::singleShot(delay, this, &SettingsBackupOperation::advanceOperationState);
}

void SettingsBackupOperation::onSubOperationError(AbstractOperation *operation)
{
    finishWithError(operation->error(), operation->errorString());
}
