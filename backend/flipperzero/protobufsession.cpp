#include "protobufsession.h"

#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QPluginLoader>
#include <QLoggingCategory>

#include "protobufplugininterface.h"
#include "mainresponseinterface.h"

#include "helper/serialinithelper.h"

#include "rpc/storageinfooperation.h"
#include "rpc/storagestatoperation.h"

#include "rpc/systemdeviceinfooperation.h"
#include "rpc/systemgetdatetimeoperation.h"
#include "rpc/systemsetdatetimeoperation.h"

#include "rpc/guiscreenframeoperation.h"
#include "rpc/guistartscreenstreamoperation.h"
#include "rpc/guistopscreenstreamoperation.h"
#include "rpc/guistartvirtualdisplayoperation.h"
#include "rpc/guistopvirtualdisplayoperation.h"

Q_LOGGING_CATEGORY(LOG_SESSION, "RPC")

using namespace Flipper;
using namespace Zero;

ProtobufSession::ProtobufSession(const QSerialPortInfo &portInfo, QObject *parent):
    QObject(parent),
    m_sessionState(Stopped),
    m_portInfo(portInfo),
    m_serialPort(nullptr),
    m_plugin(nullptr),
    m_currentOperation(nullptr),
    m_counter(0),
    m_versionMajor(0),
    m_versionMinor(0)
{}

ProtobufSession::~ProtobufSession()
{
    stopSession();
}

ProtobufSession::SessionState ProtobufSession::sessionState() const
{
    return m_sessionState;
}

void ProtobufSession::setSerialPort(const QSerialPortInfo &portInfo)
{
    m_portInfo = portInfo;
}

void ProtobufSession::setMajorVersion(int versionMajor)
{
    // TODO: unload the previous plugin and load a proper one instead
    m_versionMajor = versionMajor;
}

void ProtobufSession::setMinorVersion(int versionMinor)
{
    // TODO: change the plugin settings accordingly
    m_versionMinor = versionMinor;
}

SystemGetDateTimeOperation *ProtobufSession::getDateTime()
{
    return enqueueOperation(new SystemGetDateTimeOperation(getAndIncrementCounter(), this));
}

SystemSetDateTimeOperation *ProtobufSession::setDateTime(const QDateTime &dateTime)
{
    return enqueueOperation(new SystemSetDateTimeOperation(getAndIncrementCounter(), dateTime, this));
}

SystemDeviceInfoOperation *ProtobufSession::systemDeviceInfo()
{
    return enqueueOperation(new SystemDeviceInfoOperation(getAndIncrementCounter(), this));
}

StorageInfoOperation *ProtobufSession::storageInfo(const QByteArray &path)
{
    return enqueueOperation(new StorageInfoOperation(getAndIncrementCounter(), path, this));
}

StorageStatOperation *ProtobufSession::storageStat(const QByteArray &path)
{
    return enqueueOperation(new StorageStatOperation(getAndIncrementCounter(), path, this));
}

GuiStartScreenStreamOperation *ProtobufSession::guiStartScreenStream()
{
    return enqueueOperation(new GuiStartScreenStreamOperation(getAndIncrementCounter(), this));
}

GuiStopScreenStreamOperation *ProtobufSession::guiStopScreenStream()
{
    return enqueueOperation(new GuiStopScreenStreamOperation(getAndIncrementCounter(), this));
}

GuiStartVirtualDisplayOperation *ProtobufSession::guiStartVirtualDisplay(const QByteArray &screenData)
{
    return enqueueOperation(new GuiStartVirtualDisplayOperation(getAndIncrementCounter(), screenData, this));
}

GuiStopVirtualDisplayOperation *ProtobufSession::guiStopVirtualDisplay()
{
    return enqueueOperation(new GuiStopVirtualDisplayOperation(getAndIncrementCounter(), this));
}

GuiScreenFrameOperation *ProtobufSession::guiSendScreenFrame(const QByteArray &screenData)
{
    return enqueueOperation(new GuiScreenFrameOperation(getAndIncrementCounter(), screenData, this));
}

void ProtobufSession::startSession()
{
    if(m_sessionState != Stopped) {
        qCWarning(LOG_SESSION) << "RPC session is already running";
        return;
    }

    clearError();

    qCInfo(LOG_SESSION) << "Starting RPC session...";
    setSessionState(Starting);

    if(!loadProtobufPlugin()) {
        setSessionState(Stopped);
    }

    auto *helper = new SerialInitHelper(m_portInfo, this);
    connect(helper, &SerialInitHelper::finished, this, [=]() {
        helper->deleteLater();

        if(helper->isError()) {
            qCCritical(LOG_SESSION).noquote() << "Failed to start RPC session:" << helper->errorString();
            setError(helper->error(), helper->errorString());
            setSessionState(Stopped);
            return;
        }

        m_serialPort = helper->serialPort();

        connect(m_serialPort, &QSerialPort::readyRead, this, &ProtobufSession::onSerialPortReadyRead);
        connect(m_serialPort, &QSerialPort::bytesWritten, this, &ProtobufSession::onSerialPortBytesWriten);
        connect(m_serialPort, &QSerialPort::errorOccurred, this, &ProtobufSession::onSerialPortErrorOccured);

        qCInfo(LOG_SESSION) << "RPC session started successfully.";
        setSessionState(Idle);
    });
}

void ProtobufSession::stopSession()
{
    if(m_sessionState != Idle && m_sessionState != Running) {
        return;
    }

    // Stop session asynchronously in order to give processQueue() time to finish its job
    QTimer::singleShot(0, this, &ProtobufSession::doStopSession);
}

void ProtobufSession::onSerialPortReadyRead()
{
    if((sessionState() != Running) && (sessionState() != Idle)) {
        m_serialPort->clear();
        return;
    }

    m_receivedData.append(m_serialPort->readAll());

    auto *response = m_plugin->decode(m_receivedData, this);

    if(!response) {
        return;
    }

    auto *mainResponse = qobject_cast<MainResponseInterface*>(response);
    m_receivedData.remove(0, mainResponse->encodedSize());

    if(mainResponse->id() == m_currentOperation->id()) {
        processMatchedResponse(response);
    } else if(mainResponse->id() == 0) {
        processBroadcastResponse(response);
    } else {
        processUnmatchedResponse(response);
    }

    response->deleteLater();

    if(!m_receivedData.isEmpty()) {
        // m_receivedData can contain more than 1 full message
        QTimer::singleShot(0, this, &ProtobufSession::onSerialPortReadyRead);
    }
}

void ProtobufSession::onSerialPortBytesWriten(qint64 nbytes)
{
    m_bytesToWrite -= nbytes;

    if(m_bytesToWrite && m_currentOperation->hasNext()) {
        // Write the next part if applicable
        QTimer::singleShot(0, this, &ProtobufSession::writeToPort);
    }
}

void ProtobufSession::onSerialPortErrorOccured()
{
    qDebug() << "Serial port error occured:" << m_serialPort->errorString();
}

void ProtobufSession::processQueue()
{
    if(m_currentOperation) {
        m_currentOperation->deleteLater();
        m_currentOperation = nullptr;
    }

    if(m_queue.isEmpty()) {
        setSessionState(Idle);
        return;
    }

    m_currentOperation = m_queue.dequeue();
    qCInfo(LOG_SESSION).noquote() << prettyOperationDescription() << "START";

    writeToPort();
}

void ProtobufSession::writeToPort()
{
    const auto &buf = m_currentOperation->encodeRequest(m_plugin);
    m_bytesToWrite = m_serialPort->write(buf);
    // TODO: Check for full system serial buffer
    const auto success = (m_bytesToWrite == buf.size()) && m_serialPort->flush();

    if(!success) {
        setError(BackendError::SerialError, m_serialPort->errorString());
        stopSession();
    }
}

void ProtobufSession::doStopSession()
{
    qCInfo(LOG_SESSION) << "Stopping RPC session...";
    setSessionState(Stopping);

    clearOperationQueue();

    if(m_serialPort) {
        m_serialPort->close();
        m_serialPort->deleteLater();
    }

    unloadProtobufPlugin();

    qCInfo(LOG_SESSION) << "RPC session stopped successfully.";
    setSessionState(Stopped);
}

bool ProtobufSession::loadProtobufPlugin()
{
    QPluginLoader loader(protobufPluginPath());

    if(loader.isLoaded()) {
        return true;
    } else if(!loader.load()) {
        qCCritical(LOG_SESSION) << "Failed to load protobuf plugin:" << loader.errorString();
    } else if(!(m_plugin = qobject_cast<ProtobufPluginInterface*>(loader.instance()))) {
        qCCritical(LOG_SESSION) << "Loaded plugin does not provide the interface required";
    } else {
        return true;
    }

    setError(BackendError::UnknownError, QStringLiteral("Failed to load protobuf plugin"));
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

void ProtobufSession::clearOperationQueue()
{
    if(m_currentOperation) {
        m_currentOperation->abort(QStringLiteral("RPC session was stopped with operations still running"));
        m_currentOperation->deleteLater();
    }

    while(!m_queue.isEmpty()) {
        m_queue.dequeue()->deleteLater();
    }
}

const QString ProtobufSession::protobufPluginPath() const
{
#if defined(Q_OS_WINDOWS)
    return QString();
#elif defined(Q_OS_MAC)
    return QString();
#elif defined(Q_OS_LINUX)
    return QStringLiteral("plugins/libprotobuf%1.so").arg(m_versionMajor);
#else
#error "Unsupported OS"
#endif
}

const QString ProtobufSession::prettyOperationDescription() const
{
    return QStringLiteral("(%1) %2").arg(m_currentOperation->id()).arg(m_currentOperation->description());
}

void ProtobufSession::processMatchedResponse(QObject *response)
{
    m_currentOperation->feedResponse(response);

    if(!m_currentOperation->isFinished()) {
        return;
    } else if(m_currentOperation->isError()) {
        qCCritical(LOG_SESSION).noquote() << prettyOperationDescription() << "ERROR:" << m_currentOperation->errorString();
    } else {
        qCInfo(LOG_SESSION).noquote() << prettyOperationDescription() << "SUCCESS";
    }

    QTimer::singleShot(0, this, &ProtobufSession::processQueue);
}

void ProtobufSession::processBroadcastResponse(QObject *response)
{
    emit broadcastResponseReceived(response);
}

void ProtobufSession::processUnmatchedResponse(QObject *response)
{
    auto *mainResponse = qobject_cast<MainResponseInterface*>(response);
    qCWarning(LOG_SESSION) << "Cannot match message with id" << mainResponse->id();
}

void ProtobufSession::processErrorResponse(QObject *response)
{
    auto *mainResponse = qobject_cast<MainResponseInterface*>(response);
    qCCritical(LOG_SESSION) << "Device replied with error:" << mainResponse->errorString();
}

template<class T>
T *ProtobufSession::enqueueOperation(T *operation)
{
    m_queue.enqueue(operation);

    if(sessionState() == Idle) {
        setSessionState(Running);
        QTimer::singleShot(0, this, &ProtobufSession::processQueue);
    }

    return operation;
}
