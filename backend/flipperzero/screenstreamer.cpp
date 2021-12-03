#include "screenstreamer.h"

#include <QDebug>
#include <QSerialPort>
#include <QLoggingCategory>

#include "devicestate.h"

Q_LOGGING_CATEGORY(CATEGORY_SCREEN, "SCREEN")

using namespace Flipper;
using namespace Zero;

ScreenStreamer::ScreenStreamer(DeviceState *deviceState, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_serialPort(nullptr),
    m_isEnabled(false),
    m_isHeaderFound(false)
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
    const char input[] = { 27, 'i', (char)key, (char)type };
    m_serialPort->write(input, sizeof(input));
    m_serialPort->flush();
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
    static const auto header = QByteArrayLiteral("\xf0\xe1\xd2\xc3");

    m_dataBuffer += m_serialPort->readAll();

    if (!m_isHeaderFound) {
        const int pos = m_dataBuffer.indexOf(header);
        if (pos >= 0) {
            m_dataBuffer = m_dataBuffer.right(m_dataBuffer.length() - pos - header.size());
            m_isHeaderFound = true;
        }
    }

    if(m_isHeaderFound) {
        const auto bufSize = screenWidth() * screenHeight() / 8;

        if(m_dataBuffer.size() >= bufSize) {
            m_screenData = m_dataBuffer.left(bufSize);
            m_dataBuffer = m_dataBuffer.right(m_dataBuffer.length() - bufSize);
            m_isHeaderFound = false;

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

bool ScreenStreamer::openPort()
{
    const auto success = m_serialPort->open(QIODevice::ReadWrite);

    if(success) {
        connect(m_serialPort, &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);
        connect(m_serialPort, &QSerialPort::errorOccurred, this, &ScreenStreamer::onPortErrorOccured);

        m_serialPort->setDataTerminalReady(true);
        m_serialPort->write("\rscreen_stream\r");

    } else {
        qCDebug(CATEGORY_SCREEN).noquote() << "Failed to open serial port:" << m_serialPort->errorString();
    }

    return success;
}

void ScreenStreamer::closePort()
{
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &ScreenStreamer::onPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &ScreenStreamer::onPortErrorOccured);

    m_serialPort->write("\x01\r\n");
    m_serialPort->clear();
    m_serialPort->close();
}
