#include "flipperzeroremote.h"

#include <QSerialPort>

#include "macros.h"

namespace Flipper {

ZeroRemote::ZeroRemote(QSerialPort *port, QObject *parent):
    QObject(parent),
    m_port(port),
    m_isEnabled(false),
    m_isHeaderFound(false)
{}

ZeroRemote::~ZeroRemote()
{
    setEnabled(false);
}

const QByteArray &ZeroRemote::screenData() const
{
    return m_screenData;
}

bool ZeroRemote::isEnabled() const
{
    return m_isEnabled;
}

void ZeroRemote::setEnabled(bool enabled)
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

int ZeroRemote::screenWidth()
{
    return 128;
}

int ZeroRemote::screenHeight()
{
    return 64;
}

void ZeroRemote::onPortReadyRead()
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

void ZeroRemote::onPortErrorOccured()
{
    setEnabled(false);
}

bool ZeroRemote::openPort()
{
    const auto success = m_port->open(QIODevice::ReadWrite);

    if(success) {
        connect(m_port, &QSerialPort::readyRead, this, &ZeroRemote::onPortReadyRead);
        connect(m_port, &QSerialPort::errorOccurred, this, &ZeroRemote::onPortErrorOccured);

        m_port->setDataTerminalReady(true);
        m_port->write("\rscreen_stream\r");
    }

    return success;
}

void ZeroRemote::closePort()
{
    disconnect(m_port, &QSerialPort::readyRead, this, &ZeroRemote::onPortReadyRead);
    disconnect(m_port, &QSerialPort::errorOccurred, this, &ZeroRemote::onPortErrorOccured);

    m_port->write("\0");
    m_port->clear();
    m_port->close();
}

} // namespace Flipper
