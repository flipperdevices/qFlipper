#include "flipperzero.h"

#include "devicestate.h"
#include "firmwareupdater.h"
#include "screenstreamer.h"
#include "commandinterface.h"

using namespace Flipper;
using namespace Zero;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),
    m_state(new DeviceState(info, this)),
    m_rpc(new CommandInterface(m_state, this)),
    m_updater(new FirmwareUpdater(m_state, m_rpc, this)),
    m_streamer(new ScreenStreamer(m_rpc, this))
{
    connect(m_updater, &SignalingFailable::errorOccured, this, &FlipperZero::onErrorOccured);
    m_streamer->setEnabled(!m_state->isRecoveryMode());
}

FlipperZero::~FlipperZero()
{
    m_state->setOnline(false);
}

DeviceState *FlipperZero::deviceState() const
{
    return m_state;
}

Flipper::Zero::ScreenStreamer *FlipperZero::streamer() const
{
    return m_streamer;
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
