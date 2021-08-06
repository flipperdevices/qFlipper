#include "remotecontroller.h"

#include <QSerialPort>

#include "macros.h"

namespace Flipper {
namespace Zero {

RemoteController::RemoteController(const QSerialPortInfo &portInfo, QObject *parent):
    QObject(parent),
    m_port(new QSerialPort(portInfo, this)),
    m_isEnabled(false),
    m_isHeaderFound(false)
{}

RemoteController::~RemoteController()
{
    setEnabled(false);
}

const QByteArray &RemoteController::screenData() const
{
    return m_screenData;
}

bool RemoteController::isEnabled() const
{
    return m_isEnabled;
}

void RemoteController::setEnabled(bool enabled)
{
    if (m_isEnabled == enabled) {
        return;
    }

    if(enabled) {
        const auto success = openPort();
        check_return_void(success, "Failed to open serial port");
    } else {
        closePort();
    }

    m_isEnabled = enabled;
    emit enabledChanged();
}

int RemoteController::screenWidth()
{
    return 128;
}

int RemoteController::screenHeight()
{
    return 64;
}

void RemoteController::sendInputEvent(InputKey key, InputType type)
{
    const char input[] = { 27, 'i', (char)key, (char)type };
    m_port->write(input, sizeof(input));
    m_port->flush();
}

void RemoteController::onPortReadyRead()
{
    static const auto header = QByteArray::fromHex("F0E1D2C3");

    m_dataBuffer += m_port->readAll();

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

void RemoteController::onPortErrorOccured()
{
    setEnabled(false);
}

bool RemoteController::openPort()
{
    const auto success = m_port->open(QIODevice::ReadWrite);

    if(success) {
        connect(m_port, &QSerialPort::readyRead, this, &RemoteController::onPortReadyRead);
        connect(m_port, &QSerialPort::errorOccurred, this, &RemoteController::onPortErrorOccured);

        m_port->setDataTerminalReady(true);
        m_port->write("\rscreen_stream\r");
    }

    return success;
}

void RemoteController::closePort()
{
    disconnect(m_port, &QSerialPort::readyRead, this, &RemoteController::onPortReadyRead);
    disconnect(m_port, &QSerialPort::errorOccurred, this, &RemoteController::onPortErrorOccured);

    m_port->write("\x01\r");
    m_port->flush();
    m_port->clear();
    m_port->close();
}

} // namespace Zero
} // namespace Flipper
