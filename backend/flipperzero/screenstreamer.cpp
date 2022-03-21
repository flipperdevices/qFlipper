#include "screenstreamer.h"

#include <QDebug>
#include <QLoggingCategory>

#include "flipperzero.h"
#include "devicestate.h"
#include "protobufsession.h"
#include "guiresponseinterface.h"

#include "rpc/guistartscreenstreamoperation.h"
#include "rpc/guistopscreenstreamoperation.h"
#include "rpc/guisendinputoperation.h"

#include "pixmaps/default.h"

Q_LOGGING_CATEGORY(CATEGORY_SCREEN, "SCR")

using namespace Flipper;
using namespace Zero;

// ScreenStream and VirtualDisplay formats differ
static QByteArray transposeImage(const QByteArray &in, int width, int height)
{
    QByteArray out((width * height) / 8, 0x0);

    for(auto y = 0; y < height; ++y) {
        for(auto x = 0; x < width; ++x) {
            const auto ii = (y * width + x) / 8;
            const auto oi = y / 8 * width + x;

            const auto iz = x % 8;
            const auto oz = y % 8;

            if(in[ii] & (1 << iz)) {
                out[oi] = out[oi] | (1 << oz);
            }
        }
    }

    return out;
}

ScreenStreamer::ScreenStreamer(QObject *parent):
    QObject(parent),
    m_streamState(StreamState::Stopped),
    m_device(nullptr)
{}

void ScreenStreamer::setDevice(FlipperZero *device)
{
    if(device == m_device) {
        return;
    }

    m_device = device;
    setScreenData(transposeImage(QByteArray((char*)default_bits, sizeof(default_bits)), default_width, default_height));

    if(device) {
        auto *rpc = device->rpc();
        connect(rpc, &ProtobufSession::broadcastResponseReceived, this, &ScreenStreamer::onBroadcastResponseReceived);
        connect(rpc, &ProtobufSession::sessionStateChanged, this, &ScreenStreamer::onProtobufSessionStateChanged);
    }
}

void ScreenStreamer::sendInputEvent(int key, int type)
{
    auto *operation = m_device->rpc()->guiSendInput(key, type);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            setStreamState(Stopped);
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to send input event: " << operation->errorString();
        }
    });
}

bool ScreenStreamer::isEnabled() const
{
    return m_streamState == Running;
}

void ScreenStreamer::setEnabled(bool set)
{
    if(set) {
        start();
    } else {
        stop();
    }
}

const QSize ScreenStreamer::screenSize()
{
    return QSize(128, 64);
}

const QByteArray &ScreenStreamer::screenData() const
{
    return m_screenData;
}

void ScreenStreamer::start()
{
    if(!m_device) {
        return;
    } else if(m_streamState != StreamState::Stopped) {
        qCDebug(CATEGORY_SCREEN) << "Screen streaming is already running";
        return;
    }

    setStreamState(StreamState::Starting);

    auto *operation = m_device->rpc()->guiStartScreenStream();

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
        qCDebug(CATEGORY_SCREEN) << "Screen streaming is already stopped";
        return;
    }

    setStreamState(StreamState::Stopping);

    auto *operation = m_device->rpc()->guiStopScreenStream();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to stop screen streaming: " << operation->errorString();
        }

        setStreamState(Stopped);
    });
}

void ScreenStreamer::onProtobufSessionStateChanged()
{
    if(!m_device->rpc()->isSessionUp()) {
        setStreamState(Stopped);
    }
}

void ScreenStreamer::onBroadcastResponseReceived(QObject *response)
{
    auto *screenFrameResponse = qobject_cast<GuiScreenFrameResponseInterface*>(response);

    if(screenFrameResponse) {
        setScreenData(screenFrameResponse->screenFrame());
    }
}

void ScreenStreamer::setStreamState(StreamState newState)
{
    if(newState == m_streamState) {
        return;
    }

    m_streamState = newState;
    emit streamStateChanged();
}

void ScreenStreamer::setScreenData(const QByteArray &data)
{
    m_screenData = data;
    emit screenDataChanged();
}
