#include "screenstreamer.h"

#include <QDebug>
#include <QLoggingCategory>

#include "devicestate.h"
#include "protobufsession.h"
#include "guiresponseinterface.h"

#include "rpc/guistartscreenstreamoperation.h"
#include "rpc/guistopscreenstreamoperation.h"
#include "rpc/guisendinputoperation.h"

Q_LOGGING_CATEGORY(CATEGORY_SCREEN, "SCREEN")

using namespace Flipper;
using namespace Zero;

ScreenStreamer::ScreenStreamer(DeviceState *deviceState, ProtobufSession *rpc, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_streamState(StreamState::Stopped),
    m_rpc(rpc)
{}

void ScreenStreamer::sendInputEvent(int key, int type)
{
    auto *operation = m_rpc->guiSendInput(key, type);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            setStreamState(Stopped);
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to send input event: " << operation->errorString();
        }
    });
}

void ScreenStreamer::start()
{
    if(m_streamState != StreamState::Stopped) {
        qCDebug(CATEGORY_SCREEN) << "Can't start while already running";
        return;
    }

    setStreamState(StreamState::Starting);

    auto *operation = m_rpc->guiStartScreenStream();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            setStreamState(Stopped);
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate screen streaming: " << operation->errorString();

        } else {
            setStreamState(Running);
        }
    });
}

void ScreenStreamer::stop()
{
    if(m_streamState != StreamState::Running) {
        qCDebug(CATEGORY_SCREEN) << "Can't stop while not running";
        return;
    }

    setStreamState(StreamState::Stopping);

    auto *operation = m_rpc->guiStopScreenStream();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to stop screen streaming: " << operation->errorString();
        }

        setStreamState(Stopped);
    });
}

void ScreenStreamer::onBroadcastResponseReceived(QObject *response)
{
    auto *screenFrameResponse = qobject_cast<GuiScreenFrameResponseInterface*>(response);

    if(screenFrameResponse) {
        m_deviceState->setScreenData(screenFrameResponse->screenFrame());
    }
}

void ScreenStreamer::setStreamState(StreamState newState)
{
    if(newState == m_streamState) {
        return;
    }

    m_streamState = newState;

    if(m_streamState == StreamState::Running) {
        m_deviceState->setStreamingEnabled(true);
    } else if(m_streamState == StreamState::Stopped) {
        m_deviceState->setStreamingEnabled(false);
    }
}
