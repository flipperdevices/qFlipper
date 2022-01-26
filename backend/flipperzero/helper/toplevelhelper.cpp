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
        // Maintaining the single point of exit (ugly)
        // TODO: Refactor the code to use finished signal properly
        m_device->deviceState()->setError(BackendError::InternetError, QStringLiteral("Failed to retreive update information"));
        emit m_device->operationFinished();

    } else if(m_updateRegistry->state() == UpdateRegistry::State::Ready) {
        disconnect(m_updateRegistry, &UpdateRegistry::stateChanged, this, &AbstractTopLevelHelper::onUpdateRegistryStateChanged);
        advanceState();
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
