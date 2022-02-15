#include "protobufsession.h"

#include <QDebug>
#include <QSerialPort>
#include <QPluginLoader>
#include <QLoggingCategory>

#include "protobufplugininterface.h"
#include "mainresponseinterface.h"

Q_LOGGING_CATEGORY(LOG_SESSION, "SESSION")

using namespace Flipper;
using namespace Zero;

ProtobufSession::ProtobufSession(const QSerialPortInfo &serialInfo, QObject *parent):
    QObject(parent),
    m_sessionState(Stopped),
    m_messageID(0)
{
    setSerialPort(serialInfo);
}

ProtobufSession::SessionState ProtobufSession::sessionState() const
{
    return m_sessionState;
}

void ProtobufSession::setSerialPort(const QSerialPortInfo &serialInfo)
{
    Q_UNUSED(serialInfo)
}

void ProtobufSession::start()
{

}

void ProtobufSession::stop()
{

}

void ProtobufSession::onSerialPortBytesWriten()
{

}

void ProtobufSession::onSerialPortReadyRead()
{

}

void ProtobufSession::onSerialPortErrorOccured()
{

}

void ProtobufSession::setSessionState(SessionState newState)
{
    if(m_sessionState == newState) {
        return;
    }

    m_sessionState = newState;
    emit sessionStateChanged();
}
