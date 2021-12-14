#include "screenstreamer.h"

#include <QDebug>
#include <QSerialPort>
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
    m_serialPort(deviceState->deviceInfo().serialPort),
    m_isEnabled(false)
{
//    connect(m_deviceState, &DeviceState::deviceInfoChanged, this, &ScreenStreamer::createPort);
//    createPort();
}

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
    if(!m_serialPort || (m_isEnabled == enabled)) {
        return;
    }

    if(enabled) {
//        openPort();
    } else {
//        closePort();
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
    GuiSendInputRequest request(m_serialPort, (PB_Gui_InputKey)key, (PB_Gui_InputType)type);
    request.send();
}

void ScreenStreamer::onPortReadyRead()
{
    GuiScreenFrameResponse msg(m_serialPort);

    while(msg.receive()) {

        if(!msg.isOk()) {
            qCCritical(CATEGORY_SCREEN) << "Device replied with error:" << msg.commandStatus();
            setEnabled(false);
            return;

        } else if(!msg.isValidType()) {
            if(msg.whichContent() != MainEmptyResponse::tag()) {
                qCCritical(CATEGORY_SCREEN) << "Expected screen frame or empty, got something else:" << msg.whichContent();
                setEnabled(false);
                return;
            }

        } else {
            m_screenData = msg.screenFrame();
            emit screenDataChanged();
        }
    }
}

void ScreenStreamer::onPortErrorOccured()
{
    if(m_serialPort->error() == QSerialPort::ResourceError) {
        return;
    }

    qCDebug(CATEGORY_SCREEN).noquote() << "Serial port error occured:" << m_serialPort->errorString();
    setEnabled(false);
}

//bool ScreenStreamer::openPort()
//{
//    const auto success = m_serialPort->open(QIODevice::ReadWrite);

//    if(!success) {
//        qCDebug(CATEGORY_SCREEN).noquote() << "Failed to open serial port:" << m_serialPort->errorString();
//    } else {
//        skipMOTD();
//    }

//    return success;
//}

//void ScreenStreamer::closePort()
//{
//    disconnect(m_serialPort, &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);
//    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &ScreenStreamer::onPortErrorOccured);

//    auto *operation = new StopRPCOperation(m_serialPort, this);

//    connect(operation, &AbstractOperation::finished, this, [=]() {
//        if(operation->isError()) {
//            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to stop screen streaming: Failed to stop RPC session:" << operation->errorString();
//        }

//        m_serialPort->clear();
//        m_serialPort->close();

//        m_isEnabled = false;
//        emit enabledChanged();

//        operation->deleteLater();
//    });

//    operation->start();

//}

void ScreenStreamer::skipMOTD()
{
    auto *operation = new SkipMOTDOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate screen streaming: Failed to skip MOTD:" << operation->errorString();
        } else {
            startRPCSession();
        }

        operation->deleteLater();
    });

    operation->start();
}

void ScreenStreamer::startRPCSession()
{
    auto *operation = new StartRPCOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate screen streaming: Failed to start RPC session:" << operation->errorString();
        } else {
            startScreenStream();
        }

        operation->deleteLater();
    });

    operation->start();
}

void ScreenStreamer::startScreenStream()
{
    auto *operation = new StartStreamOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate Screen Streaming: Failed to execute StartStreamOperation:" << operation->errorString();
        } else {
            connect(m_serialPort, &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);
            connect(m_serialPort, &QSerialPort::errorOccurred, this, &ScreenStreamer::onPortErrorOccured);

            m_isEnabled = true;
            emit enabledChanged();
        }

        operation->deleteLater();
    });

    operation->start();
}
