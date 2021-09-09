#include "motdskipper.h"

#include <QTimer>
#include <QSerialPort>

#define RESPONSE_TIMEOUT_MS 5000
#define PROMPT_READY "\r\n>: "

using namespace Flipper;
using namespace Zero;

MOTDSkipper::MOTDSkipper(QSerialPort *serialPort, QObject *parent):
    QObject(parent),
    m_responseTimer(new QTimer(this)),
    m_serialPort(serialPort)
{
    m_responseTimer->setInterval(1000);
    m_responseTimer->setSingleShot(true);

    connect(m_responseTimer, &QTimer::timeout, this, &MOTDSkipper::onResponseTimeout);
    QTimer::singleShot(0, this, &MOTDSkipper::begin);
}

void MOTDSkipper::begin()
{
    if(!m_serialPort->setDataTerminalReady(true)) {
        finishWithError(m_serialPort->errorString());
    } else {
        connect(m_serialPort, &QSerialPort::readyRead, this, &MOTDSkipper::onSerialPortReadyRead);
        connect(m_serialPort, &QSerialPort::errorOccurred, this, &MOTDSkipper::onSerialPortErrorOccured);

        m_responseTimer->start(RESPONSE_TIMEOUT_MS);
    }
}

void MOTDSkipper::onSerialPortReadyRead()
{
    m_responseTimer->start(RESPONSE_TIMEOUT_MS);
    m_receivedData += m_serialPort->readAll();

    if(m_receivedData.endsWith(PROMPT_READY)) {
        finish();
    }
}

void MOTDSkipper::onSerialPortErrorOccured()
{
    finishWithError(m_serialPort->errorString());
}

void MOTDSkipper::onResponseTimeout()
{
    finishWithError(QStringLiteral("Serial port response timeout"));
}

void MOTDSkipper::finish()
{
    m_responseTimer->stop();

    disconnect(m_serialPort, &QSerialPort::readyRead, this, &MOTDSkipper::onSerialPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &MOTDSkipper::onSerialPortErrorOccured);

    emit finished();
}

void MOTDSkipper::finishWithError(const QString &errorMsg)
{
    setError(errorMsg);
    finish();
}
