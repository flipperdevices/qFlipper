#include "toplevelhelper.h"

#include "updateregistry.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"
#include "flipperzero/screenstreamer.h"
#include "flipperzero/firmwareupdater.h"

#include "flipperzero/toplevel/abstracttopleveloperation.h"

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

void AbstractTopLevelHelper::onUpdatesChecked()
{
    disconnect(m_updateRegistry, &UpdateRegistry::channelsChanged, this, &AbstractTopLevelHelper::onUpdatesChecked);

    if(!m_updateRegistry->isReady()) {
        finishWithError(QStringLiteral("Failed to retreive update information"));
    } else{
        advanceState();
    }
}

void AbstractTopLevelHelper::onStreamStateChanged()
{
    disconnect(m_device->streamer(), &ScreenStreamer::stateChanged, this, &AbstractTopLevelHelper::onStreamStateChanged);
    advanceState();
}

void AbstractTopLevelHelper::nextStateLogic()
{
    if(state() == AbstractOperationHelper::Ready) {
        setState(AbstractTopLevelHelper::CheckingForUpdates);
        checkForUpdates();

    } else if(state() == AbstractTopLevelHelper::CheckingForUpdates) {
        setState(AbstractTopLevelHelper::RunningCustomOperation);
        runCustomOperation();

    } else if(state() == AbstractTopLevelHelper::RunningCustomOperation) {
        finish();
    }
}

void AbstractTopLevelHelper::checkForUpdates()
{
    m_device->deviceState()->setStatusString(tr("Checking for updates..."));

    connect(m_updateRegistry, &UpdateRegistry::channelsChanged, this, &AbstractTopLevelHelper::onUpdatesChecked);
    m_updateRegistry->check();
}

UpdateTopLevelHelper::UpdateTopLevelHelper(UpdateRegistry *updateRegistry, FlipperZero *device, QObject *parent):
    AbstractTopLevelHelper(updateRegistry, device, parent)
{}

void UpdateTopLevelHelper::runCustomOperation()
{
    auto &versionInfo = updateRegistry()->latestVersion();
    device()->updateOrRepair(versionInfo);
}
