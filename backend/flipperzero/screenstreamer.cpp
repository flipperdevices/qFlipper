#include "screenstreamer.h"

#include <QDebug>
#include <QLoggingCategory>

#include "cli/stoprpcoperation.h"
#include "cli/skipmotdoperation.h"
#include "cli/startrpcoperation.h"
#include "cli/startstreamoperation.h"

#include "protobuf/guiprotobufmessage.h"
#include "devicestate.h"

Q_LOGGING_CATEGORY(CATEGORY_SCREEN, "SCREEN")

using namespace Flipper;
using namespace Zero;

ScreenStreamer::ScreenStreamer(DeviceState *deviceState, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_isEnabled(false)
{}

ScreenStreamer::~ScreenStreamer()
{
    setEnabled(false);
}

const QByteArray &ScreenStreamer::screenData() const
{
    return m_screenData;
}

bool ScreenStreamer::isEnabled() const
{
    return m_isEnabled;
}

void ScreenStreamer::setEnabled(bool enabled)
{
    if(m_isEnabled == enabled) {
        return;
    } else if(m_isEnabled) {
        start();
    } else {
        stop();
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
                setEnabled(false);
                return;
            }

        } else {
            m_screenData = msg.screenFrame();
            emit screenDataChanged();
        }
    }
}

void ScreenStreamer::start()
{
    auto *operation = new StartStreamOperation(serialPort(), this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate Screen Streaming: Failed to execute StartStreamOperation:" << operation->errorString();
        } else {
            connect(serialPort(), &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);

            m_isEnabled = true;
            emit enabledChanged();
        }

        operation->deleteLater();
    });

    operation->start();
}

void ScreenStreamer::stop()
{

}

QSerialPort *ScreenStreamer::serialPort() const
{
    return m_deviceState->deviceInfo().serialPort.get();
}
