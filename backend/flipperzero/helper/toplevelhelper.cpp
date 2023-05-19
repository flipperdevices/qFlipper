#include "toplevelhelper.h"

#include "updateregistry.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"

using namespace Flipper;
using namespace Zero;

AbstractTopLevelHelper::AbstractTopLevelHelper(UpdateRegistry *updateRegistry, FlipperZero *device, QObject *parent):
    AbstractOperationHelper(parent),
    m_updateRegistry(updateRegistry),
    m_device(device)
{
    m_device->deviceState()->setProgress(-1);
    m_device->deviceState()->setPersistent(true);

    connect(m_device, &FlipperZero::operationFinished, this, &AbstractTopLevelHelper::finish);
}

UpdateRegistry *AbstractTopLevelHelper::updateRegistry()
{
    return m_updateRegistry;
}

FlipperZero *AbstractTopLevelHelper::device()
{
    return m_device;
}

void AbstractTopLevelHelper::onUpdateRegistryStateChanged()
{
    if(m_updateRegistry->state() == UpdateRegistry::State::ErrorOccured) {
        finishEarly(BackendError::InternetError, QStringLiteral("Failed to retrieve update information"));
    } else if(m_updateRegistry->state() == UpdateRegistry::State::Ready) {
        disconnect(m_updateRegistry, &UpdateRegistry::stateChanged, this, &AbstractTopLevelHelper::onUpdateRegistryStateChanged);

        if(!m_device->deviceState()->isOnline()) {
            finishEarly(BackendError::OperationError, QStringLiteral("Connection to device was lost"));
        } else {
            advanceState();
        }
    }
}

void AbstractTopLevelHelper::nextStateLogic()
{
    if(state() == AbstractOperationHelper::Ready) {
        setState(AbstractTopLevelHelper::CheckingForUpdates);
        checkForUpdates();

    } else if(state() == AbstractTopLevelHelper::CheckingForUpdates) {
        setState(AbstractTopLevelHelper::RunningCustomOperation);
        runCustomOperation();
    }
}

void AbstractTopLevelHelper::checkForUpdates()
{
    m_device->deviceState()->setStatusString(tr("Checking for updates..."));

    connect(m_updateRegistry, &UpdateRegistry::stateChanged, this, &AbstractTopLevelHelper::onUpdateRegistryStateChanged);
    m_updateRegistry->check();
}

/* This is different from the usual finishWithError() method because it uses the deviceState to both
 * signal the end of operation and pass the error information. */
void AbstractTopLevelHelper::finishEarly(BackendError::ErrorType error, const QString &errorString)
{
    m_device->deviceState()->setError(error, errorString);
    emit m_device->operationFinished();
}

UpdateTopLevelHelper::UpdateTopLevelHelper(UpdateRegistry *updateRegistry, FlipperZero *device, QObject *parent):
    AbstractTopLevelHelper(updateRegistry, device, parent)
{}

void UpdateTopLevelHelper::runCustomOperation()
{
    auto &versionInfo = updateRegistry()->latestVersion();
    device()->fullUpdate(versionInfo);
}

RepairTopLevelHelper::RepairTopLevelHelper(UpdateRegistry *updateRegistry, FlipperZero *device, QObject *parent):
    AbstractTopLevelHelper(updateRegistry, device, parent)
{}

void RepairTopLevelHelper::runCustomOperation()
{
    auto &versionInfo = updateRegistry()->latestVersion();
    device()->fullRepair(versionInfo);
}
