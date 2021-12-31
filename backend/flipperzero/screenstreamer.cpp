#include "screenstreamer.h"

#include <QDebug>
#include <QLoggingCategory>

#include "devicestate.h"
#include "commandinterface.h"

#include "rpc/guistartstreamoperation.h"
#include "rpc/guistopstreamoperation.h"

#include "protobuf/guiprotobufmessage.h"

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
    GuiSendInputRequest request(serialPort(), (PB_Gui_InputKey)key, (PB_Gui_InputType)type);
    request.send();
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
}

void ScreenStreamer::onPortReadyRead()
{
    GuiScreenFrameResponse msg(serialPort());

    while(msg.receive()) {

        if(!msg.isOk()) {
            qCDebug(CATEGORY_SCREEN) << "Device replied with error:" << msg.commandStatusString();
            return;

        } else if(!msg.isValidType()) {
            if(msg.whichContent() != MainEmptyResponse::tag()) {
                qCDebug(CATEGORY_SCREEN) << "Expected screen frame or empty, got something else";
                return;
            }

        } else {
            m_deviceState->setScreenData(msg.screenFrame());
        }
    }

    if(m_state == State::Stopping) {
        // Stop only after processing all of the messages
        sendStopCommand();
    }
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
    m_deviceState->setStreamingEnabled(m_state == State::Running);
}

QSerialPort *ScreenStreamer::serialPort() const
{
    return m_deviceState->serialPort();
}
