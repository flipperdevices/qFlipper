#include "screenstreamer.h"

#include <QDebug>
#include <QSerialPort>
#include <QLoggingCategory>

#include "cli/skipmotdoperation.h"
#include "cli/startrpcoperation.h"
#include "cli/startstreamoperation.h"

#include "mainprotobufmessage.h"
#include "devicestate.h"

Q_LOGGING_CATEGORY(CATEGORY_SCREEN, "SCREEN")

using namespace Flipper;
using namespace Zero;

ScreenStreamer::ScreenStreamer(DeviceState *deviceState, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_serialPort(nullptr),
    m_isEnabled(false)
{
    connect(m_deviceState, &DeviceState::deviceInfoChanged, this, &ScreenStreamer::createPort);
    createPort();
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
    } else if(enabled) {
        if(!openPort()) return;
    } else {
        closePort();
    }

    m_isEnabled = enabled;
    emit enabledChanged();
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
    Q_UNUSED(key)
    Q_UNUSED(type)
//    const char input[] = { 27, 'i', (char)key, (char)type };
//    m_serialPort->write(input, sizeof(input));
//    m_serialPort->flush();
}

void ScreenStreamer::createPort()
{
    if(m_serialPort) {
        setEnabled(false);
        m_serialPort->deleteLater();
    }

    if(!m_deviceState->isRecoveryMode()) {
        m_serialPort = new QSerialPort(m_deviceState->deviceInfo().serialInfo, this);
    } else{
        m_serialPort = nullptr;
    }
}

void ScreenStreamer::onPortReadyRead()
{
    GuiScreenFrameResponse msg(m_serialPort);

    if(!msg.receive()) {
        // TODO: Distinguish incomplete mesages and broken session
        return;

    } else if(!msg.isOk()) {
        qCCritical(CATEGORY_SCREEN) << "Device replied with error:" << msg.commandStatus();
        setEnabled(false);

    } else if(!msg.isValidType()) {
        qCCritical(CATEGORY_SCREEN) << "Expected screen frame, got something else:" << msg.whichContent();
        setEnabled(false);

    } else {
        m_screenData = msg.screenFrame();
        emit screenDataChanged();
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

bool ScreenStreamer::openPort()
{
    const auto success = m_serialPort->open(QIODevice::ReadWrite);

    if(!success) {
        qCDebug(CATEGORY_SCREEN).noquote() << "Failed to open serial port:" << m_serialPort->errorString();
    } else {
        skipMOTD();
    }

    return success;
}

void ScreenStreamer::closePort()
{
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &ScreenStreamer::onPortErrorOccured);

    m_serialPort->clear();
    m_serialPort->close();
}

void ScreenStreamer::skipMOTD()
{
    auto *operation = new SkipMOTDOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate Screen Streaming:" << operation->errorString();
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
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate Screen Streaming:" << operation->errorString();
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
            qCDebug(CATEGORY_SCREEN).noquote() << "Failed to initiate Screen Streaming:" << operation->errorString();
        } else {
            connect(m_serialPort, &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);
            connect(m_serialPort, &QSerialPort::errorOccurred, this, &ScreenStreamer::onPortErrorOccured);
        }

        operation->deleteLater();
    });

    operation->start();
}
