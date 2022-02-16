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
    m_serialPort(nullptr),
    m_plugin(nullptr),
    m_counter(0),
    m_versionMajor(0),
    m_versionMinor(0)
{
    setSerialPort(serialInfo);
}

ProtobufSession::~ProtobufSession()
{
    unloadProtobufPlugin();
}

ProtobufSession::SessionState ProtobufSession::sessionState() const
{
    return m_sessionState;
}

void ProtobufSession::setSerialPort(const QSerialPortInfo &serialInfo)
{
    Q_UNUSED(serialInfo)
}

void ProtobufSession::setMajorVersion(int versionMajor)
{
    m_versionMajor = versionMajor;
}

void ProtobufSession::setMinorVersion(int versionMinor)
{
    m_versionMinor = versionMinor;
}

void ProtobufSession::start()
{
    loadProtobufPlugin();
}

void ProtobufSession::stop()
{
    unloadProtobufPlugin();
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

bool ProtobufSession::loadProtobufPlugin()
{
    QPluginLoader loader(protobufPluginPath());

    if(!loader.load()) {
        qCCritical(LOG_SESSION) << "Failed to load protobuf plugin:" << loader.errorString();
    } else if(!(m_plugin = qobject_cast<ProtobufPluginInterface*>(loader.instance()))) {
        qCCritical(LOG_SESSION) << "Loaded plugin does not provide the interface required";
    } else {
        return true;
    }

    setSessionState(ErrorOccured);
    return false;
}

bool ProtobufSession::unloadProtobufPlugin()
{
    QPluginLoader loader(protobufPluginPath());
    return loader.isLoaded() ? loader.unload() : true;
}

uint32_t ProtobufSession::getAndIncrementCounter()
{
    // Skip 0, it is reserved for broadcast messages
    do {
        ++m_counter;
    } while(m_counter == 0);

    return m_counter;
}

void ProtobufSession::setSessionState(SessionState newState)
{
    if(m_sessionState == newState) {
        return;
    }

    m_sessionState = newState;
    emit sessionStateChanged();
}

const QString ProtobufSession::protobufPluginPath() const
{
    return QStringLiteral("plugins/libprotobuf%1.so").arg(m_versionMajor);
}
