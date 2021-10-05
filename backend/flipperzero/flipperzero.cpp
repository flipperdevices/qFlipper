#include "flipperzero.h"

#include "devicestate.h"
#include "firmwareupdater.h"
#include "screenstreaminterface.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),
    m_state(new DeviceState(info, this)),
    m_updater(new FirmwareUpdater(m_state, this))
{
    connect(m_updater, &SignalingFailable::errorOccured, this, &FlipperZero::onErrorOccured);
}

FlipperZero::~FlipperZero()
{
    m_state->setOnline(false);
}

DeviceState *FlipperZero::deviceState() const
{
    return m_state;
}

Flipper::Zero::ScreenStreamInterface *FlipperZero::screen() const
{
    return m_screen;
}

FirmwareUpdater *FlipperZero::updater() const
{
    return m_updater;
}

void FlipperZero::onErrorOccured()
{
    auto *instance = qobject_cast<SignalingFailable*>(sender());
    m_state->setErrorString(instance->errorString());
}
