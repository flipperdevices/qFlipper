#include "screenstreamer.h"

#include <QDebug>
#include <QLoggingCategory>

#include "devicestate.h"
#include "commandinterface.h"

#include "cli/guistartstreamoperation.h"
#include "cli/guistopstreamoperation.h"

#include "protobuf/guiprotobufmessage.h"

Q_LOGGING_CATEGORY(CATEGORY_SCREEN, "SCREEN")

using namespace Flipper;
using namespace Zero;

ScreenStreamer::ScreenStreamer(DeviceState *deviceState, CommandInterface *rpc, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_rpc(rpc),
    m_state(State::Stopped)
{
    connect(m_deviceState, &DeviceState::isPersistentChanged, this, &ScreenStreamer::onDeviceStateChanged);
    onDeviceStateChanged();
}

const QByteArray &ScreenStreamer::screenData() const
{
    return m_screenData;
}

bool ScreenStreamer::isEnabled() const
{
    return m_state != State::Stopped;
}

void ScreenStreamer::setEnabled(bool enabled)
{
    if(enabled && m_state == State::Stopped) {
        start();

    } else if(!enabled && m_state == State::Running) {
        setState(State::Stopping);
    }
}

int ScreenStreamer::screenWidth()
{
    return 128;
}

int ScreenStreamer::screenHeight()
{
    return 64;
}

void ScreenStreamer::sendInputEvent(InputKey key, InputType type)
{
    GuiSendInputRequest request(serialPort(), (PB_Gui_InputKey)key, (PB_Gui_InputType)type);
    request.send();
}

void ScreenStreamer::onPortReadyRead()
{
    GuiScreenFrameResponse msg(serialPort());

    while(msg.receive()) {

        if(!msg.isOk()) {
            qCCritical(CATEGORY_SCREEN) << "Device replied with error:" << msg.commandStatusString();
            setEnabled(false);
            return;

        } else if(!msg.isValidType()) {
            if(msg.whichContent() != MainEmptyResponse::tag()) {
                qCCritical(CATEGORY_SCREEN) << "Expected screen frame or empty, got something else";
                return;
            }

        } else {
            m_screenData = msg.screenFrame();
            emit screenDataChanged();
        }
    }

    if(m_state == State::Stopping) {
        // Stop only after processing all of the messages
        stop();
    }
}

void ScreenStreamer::onDeviceStateChanged()
{
    setEnabled(!m_deviceState->isRecoveryMode() && !m_deviceState->isPersistent());
}

void ScreenStreamer::start()
{
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
    emit enabledChanged();
}

QSerialPort *ScreenStreamer::serialPort() const
{
    return m_rpc->serialPort();
}
