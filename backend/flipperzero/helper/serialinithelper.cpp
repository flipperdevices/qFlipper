#include "serialinithelper.h"

#include <QTimer>
#include <QSerialPort>

#include "flipperzero/rpc/skipmotdoperation.h"
#include "flipperzero/rpc/startrpcoperation.h"

using namespace Flipper;
using namespace Zero;

SerialInitHelper::SerialInitHelper(const QSerialPortInfo &portInfo, QObject *parent):
    AbstractOperationHelper(parent),
    m_serialPort(new QSerialPort(portInfo, parent)),
    m_retryTimer(new QTimer(this)),
    m_retryCount(20)
{
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &SerialInitHelper::openPort);
}

QSerialPort *SerialInitHelper::serialPort() const
{
    return m_serialPort;
}

void SerialInitHelper::nextStateLogic()
{
    if(state() == AbstractOperationHelper::Ready) {
        setState(SerialInitHelper::OpeningPort);
        openPort();

    } else if(state() == SerialInitHelper::OpeningPort) {
        setState(SerialInitHelper::SkippingMOTD);
        skipMOTD();

    } else if(state() == SerialInitHelper::SkippingMOTD) {
        setState(SerialInitHelper::StartingRPCSession);
        startRPCSession();

    } else if(state() == SerialInitHelper::StartingRPCSession) {
        finish();
    }
}

void SerialInitHelper::openPort()
{
    if(!(m_retryCount--)) {
        finishWithError(BackendError::SerialAccessError, QStringLiteral("Failed to open serial port: %1").arg(m_serialPort->errorString()));
    } else if(!m_serialPort->open(QIODevice::ReadWrite)) {
        m_retryTimer->start(std::chrono::milliseconds(50));
    } else {
        advanceState();
    }
}

void SerialInitHelper::skipMOTD()
{
    auto *operation = new SkipMOTDOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::SerialAccessError, QStringLiteral("Failed to begin CLI session: %1").arg(operation->errorString()));
        } else {
            advanceState();
        }

        operation->deleteLater();
    });

    if(m_serialPort->isDataTerminalReady()) {
        // Serial port was not reset correctly (e.g. by minicom)
        m_serialPort->setDataTerminalReady(false);
        // Wait a bit before setting DTR high again
        QTimer::singleShot(50, operation, &AbstractOperation::start);
    } else {
        operation->start();
    }
}

void SerialInitHelper::startRPCSession()
{
    auto *operation = new StartRPCOperation(m_serialPort, this);
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::SerialAccessError, QStringLiteral("Failed to start RPC session: %1").arg(operation->errorString()));
        } else {
            advanceState();
        }

        operation->deleteLater();
    });

    operation->start();
}
