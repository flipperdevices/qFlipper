#include "flipperzero.h"

#include "devicestate.h"
#include "firmwareupdater.h"
#include "screenstreaminterface.h"

#include "macros.h"

namespace Flipper {

using namespace Zero;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),

    m_state(new DeviceState(info, this)),
    m_updater(new FirmwareUpdater(m_state, this))
{
    //TODO: expose state instead?
    connect(m_state, &DeviceState::deviceInfoChanged, this, &FlipperZero::deviceInfoChanged);
    connect(m_state, &DeviceState::isPersistentChanged, this, &FlipperZero::isPersistentChanged);
    connect(m_state, &DeviceState::isOnlineChanged, this, &FlipperZero::isOnlineChanged);
    connect(m_state, &DeviceState::errorChanged, this, &FlipperZero::isErrorChanged);
    connect(m_state, &DeviceState::progressChanged, this, &FlipperZero::progressChanged);
    connect(m_state, &DeviceState::statusChanged, this, &FlipperZero::messageChanged);
}

FlipperZero::~FlipperZero()
{
    setOnline(false);
}

void FlipperZero::reset(const Zero::DeviceInfo &info)
{
    m_state->setDeviceInfo(info);
    setOnline(true);
}

void FlipperZero::setPersistent(bool set)
{
    Q_UNUSED(set)
    info_msg("I do nothing, delete me ASAP!");
}

void FlipperZero::setOnline(bool set)
{
    m_state->setOnline(set);
}

void FlipperZero::setError(const QString &msg, bool set)
{
    Q_UNUSED(msg)
    Q_UNUSED(set)

    info_msg("I do nothing, delete me ASAP!");
}

bool FlipperZero::isPersistent() const
{
    return m_state->isPersistent();
}

bool FlipperZero::isOnline() const
{
    return m_state->isOnline();
}

bool FlipperZero::isError() const
{
    return m_state->isError();
}

bool FlipperZero::bootToDFU()
{
    info_msg("I do nothing, delete me ASAP!");
    return false;
}

const QString &FlipperZero::name() const
{
    return m_state->deviceInfo().name;
}

const QString &FlipperZero::model() const
{
    static const QString m = "Flipper Zero";
    return m;
}

const QString &FlipperZero::target() const
{
    return m_state->deviceInfo().target;
}

const QString &FlipperZero::version() const
{
    const auto &deviceInfo = m_state->deviceInfo();

    if(deviceInfo.firmware.branch == QStringLiteral("dev")) {
        return deviceInfo.firmware.commit;
    } else {
        return deviceInfo.firmware.version;
    }
}

const QString &FlipperZero::messageString() const
{
    return m_state->statusString();
}

const QString &FlipperZero::errorString() const
{
    return m_state->errorString();
}

double FlipperZero::progress() const
{
    return m_state->progress();
}

const DeviceInfo &FlipperZero::deviceInfo() const
{
    return m_state->deviceInfo();
}

bool FlipperZero::isDFU() const
{
    return m_state->isRecoveryMode();
}

Flipper::Zero::ScreenStreamInterface *FlipperZero::screen() const
{
    return m_screen;
}

Recovery *FlipperZero::recovery() const
{
    info_msg("I do nothing, delete me ASAP!");
    return nullptr;
}

RecoveryInterface *FlipperZero::recoveryNew() const
{
    info_msg("I do nothing, delete me ASAP!");
    return nullptr;
}

CommandInterface *FlipperZero::cli() const
{
    info_msg("I do nothing, delete me ASAP!");
    return nullptr;
}

void FlipperZero::fullUpdate()
{
    m_updater->fullUpdate();
}

void FlipperZero::setMessage(const QString &message)
{
    Q_UNUSED(message)
    info_msg("I do nothing, delete me ASAP!");
}

void FlipperZero::setProgress(double progress)
{
    Q_UNUSED(progress)
    info_msg("I do nothing, delete me ASAP!");
}

}
