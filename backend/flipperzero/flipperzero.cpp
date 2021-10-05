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
{}

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

void FlipperZero::fullUpdate()
{
    m_updater->fullUpdate();
}
