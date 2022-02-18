#include "screenstreamer.h"

#include <QDebug>
#include <QSerialPort>
#include <QLoggingCategory>

#include "devicestate.h"
#include "commandinterface.h"

#include "rpc/guistartscreenstreamoperation.h"
#include "rpc/guistopscreenstreamoperation.h"

Q_LOGGING_CATEGORY(CATEGORY_SCREEN, "SCREEN")

using namespace Flipper;
using namespace Zero;

ScreenStreamer::ScreenStreamer(DeviceState *deviceState, CommandInterface *rpc, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_rpc(rpc),
    m_state(State::Stopped)
{}

void ScreenStreamer::sendInputEvent(int key, int type)
{
    Q_UNUSED(key)
    Q_UNUSED(type)
}

void ScreenStreamer::start()
{
    if(m_state != State::Stopped) {
        qCDebug(CATEGORY_SCREEN) << "Can't start while already running";
        return;
    }

    setState(State::Starting);

    auto *operation = m_rpc->guiStartStreaming();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            setState(State::Stopped);
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate screen streaming: " << operation->errorString();

        } else {
            setState(State::Running);
            connect(serialPort(), &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);
        }
    });
}

void ScreenStreamer::stop()
{
    if(m_state != State::Running) {
        qCDebug(CATEGORY_SCREEN) << "Can't stop while not running";
        return;
    }

    setState(State::Stopping);
    // Call this in case there is no data on the serial port
    onPortReadyRead();
}

void ScreenStreamer::onPortReadyRead()
{
}

void ScreenStreamer::sendStopCommand()
{
    disconnect(serialPort(), &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);

    auto *operation = m_rpc->guiStopStreaming();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to stop screen streaming: " << operation->errorString();
        }

        setState(State::Stopped);
    });
}

void ScreenStreamer::setState(State newState)
{
    if(newState == m_state) {
        return;
    }

    m_state = newState;

    if(m_state == State::Running) {
        m_deviceState->setStreamingEnabled(true);
    } else if(m_state == State::Stopped) {
        m_deviceState->setStreamingEnabled(false);
    }
}

QSerialPort *ScreenStreamer::serialPort() const
{
    return m_deviceState->serialPort();
}
