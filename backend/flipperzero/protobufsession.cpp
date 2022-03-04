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
#include "rpc/storagelistoperation.h"
#include "rpc/storagereadoperation.h"
#include "rpc/storagemkdiroperation.h"
#include "rpc/storagewriteoperation.h"
#include "rpc/storageremoveoperation.h"

#include "rpc/systemrebootoperation.h"
#include "rpc/systemdeviceinfooperation.h"
#include "rpc/systemgetdatetimeoperation.h"
#include "rpc/systemsetdatetimeoperation.h"
#include "rpc/systemfactoryresetoperation.h"

#include "rpc/guisendinputoperation.h"
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
    m_loader(new QPluginLoader(this)),
    m_plugin(nullptr),
    m_currentOperation(nullptr),
    m_bytesToWrite(0),
    m_counter(0),
    m_versionMajor(0),
    m_versionMinor(0)
{}

ProtobufSession::~ProtobufSession()
{
    stopSession();
}

bool ProtobufSession::isSessionUp() const
{
    return m_sessionState == Idle || m_sessionState == Running;
}

void ProtobufSession::setSerialPort(const QSerialPortInfo &portInfo)
{
    m_portInfo = portInfo;
}

void ProtobufSession::setMajorVersion(int versionMajor)
{
    m_versionMajor = versionMajor;
}

void ProtobufSession::setMinorVersion(int versionMinor)
{
    m_versionMinor = versionMinor;

    if(m_loader->isLoaded()) {
        m_plugin->setMinorVersion(m_versionMinor);
    }
}

SystemRebootOperation *ProtobufSession::rebootToOS()
{
    return enqueueOperation(new SystemRebootOperation(getAndIncrementCounter(), SystemRebootOperation::RebootModeOS, this));
}

SystemRebootOperation *ProtobufSession::rebootToRecovery()
{
    return enqueueOperation(new SystemRebootOperation(getAndIncrementCounter(), SystemRebootOperation::RebootModeRecovery, this));
}

SystemGetDateTimeOperation *ProtobufSession::getDateTime()
{
    return enqueueOperation(new SystemGetDateTimeOperation(getAndIncrementCounter(), this));
}

SystemSetDateTimeOperation *ProtobufSession::setDateTime(const QDateTime &dateTime)
{
    return enqueueOperation(new SystemSetDateTimeOperation(getAndIncrementCounter(), dateTime, this));
}

SystemFactoryResetOperation *ProtobufSession::factoryReset()
{
    return enqueueOperation(new SystemFactoryResetOperation(getAndIncrementCounter(), this));
}

SystemDeviceInfoOperation *ProtobufSession::systemDeviceInfo()
{
    return enqueueOperation(new SystemDeviceInfoOperation(getAndIncrementCounter(), this));
}

StorageListOperation *ProtobufSession::storageList(const QByteArray &path)
{
    return enqueueOperation(new StorageListOperation(getAndIncrementCounter(), path, this));
}

StorageInfoOperation *ProtobufSession::storageInfo(const QByteArray &path)
{
    return enqueueOperation(new StorageInfoOperation(getAndIncrementCounter(), path, this));
}

StorageStatOperation *ProtobufSession::storageStat(const QByteArray &path)
{
    return enqueueOperation(new StorageStatOperation(getAndIncrementCounter(), path, this));
}

StorageMkdirOperation *ProtobufSession::storageMkdir(const QByteArray &path)
{
    return enqueueOperation(new StorageMkdirOperation(getAndIncrementCounter(), path, this));
}

StorageRemoveOperation *ProtobufSession::storageRemove(const QByteArray &path)
{
    return enqueueOperation(new StorageRemoveOperation(getAndIncrementCounter(), path, this));
}

StorageReadOperation *ProtobufSession::storageRead(const QByteArray &path, QIODevice *file)
{
    return enqueueOperation(new StorageReadOperation(getAndIncrementCounter(), path, file, this));
}

StorageWriteOperation *ProtobufSession::storageWrite(const QByteArray &path, QIODevice *file)
{
    return enqueueOperation(new StorageWriteOperation(getAndIncrementCounter(), path, file, this));
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

GuiSendInputOperation *ProtobufSession::guiSendInput(int key, int type)
{
    return enqueueOperation(new GuiSendInputOperation(getAndIncrementCounter(), key, type, this));
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
    m_receivedData.clear();

    qCInfo(LOG_SESSION) << "Starting RPC session...";
    m_sessionState = Starting;

    if(!loadProtobufPlugin()) {
        stopEarly(BackendError::UnknownError, QStringLiteral("Failed to load protobuf plugin: %1").arg(m_loader->errorString()));
        return;
    }

    auto *helper = new SerialInitHelper(m_portInfo, this);
    connect(helper, &SerialInitHelper::finished, this, [=]() {
        helper->deleteLater();

        if(helper->isError()) {
            qCCritical(LOG_SESSION).noquote() << "Failed to start RPC session:" << helper->errorString();
            stopEarly(helper->error(), helper->errorString());
            return;
        }

        m_serialPort = helper->serialPort();

        connect(m_serialPort, &QSerialPort::readyRead, this, &ProtobufSession::onSerialPortReadyRead);
        connect(m_serialPort, &QSerialPort::bytesWritten, this, &ProtobufSession::onSerialPortBytesWriten);
        connect(m_serialPort, &QSerialPort::errorOccurred, this, &ProtobufSession::onSerialPortErrorOccured);

        qCInfo(LOG_SESSION) << "RPC session started successfully.";

        m_sessionState = Idle;
        emit sessionStatusChanged();
    });
}

void ProtobufSession::stopSession()
{
    if(!isSessionUp()) {
        return;
    }

    // Stop session asynchronously in order to give processQueue() time to finish its job
    QTimer::singleShot(0, this, &ProtobufSession::doStopSession);
}

void ProtobufSession::onSerialPortReadyRead()
{
    if(!isSessionUp()) {
        m_serialPort->clear();
        return;
    }

    m_receivedData.append(m_serialPort->readAll());
    auto *response = m_plugin->decode(m_receivedData, this);

    if(!response) {
        return;
    }

    auto *mainResponse = qobject_cast<MainResponseInterface*>(response);
    m_receivedData.remove(0, (int)mainResponse->encodedSize());

    if(m_currentOperation && mainResponse->id() == m_currentOperation->id()) {
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
    Q_UNUSED(nbytes)
}

void ProtobufSession::onSerialPortErrorOccured()
{
    qCInfo(LOG_SESSION) << "Serial connection was lost.";

    disconnect(m_serialPort, &QSerialPort::readyRead, this, &ProtobufSession::onSerialPortReadyRead);
    disconnect(m_serialPort, &QSerialPort::bytesWritten, this, &ProtobufSession::onSerialPortBytesWriten);
    disconnect(m_serialPort, &QSerialPort::errorOccurred, this, &ProtobufSession::onSerialPortErrorOccured);

    stopSession();
}

void ProtobufSession::processQueue()
{
    if(m_queue.isEmpty()) {
        m_sessionState = Idle;
        return;
    }

    m_currentOperation = m_queue.dequeue();
    m_currentOperation->start();

    connect(m_currentOperation, &AbstractOperation::finished, this, &ProtobufSession::onCurrentOperationFinished);

    qCInfo(LOG_SESSION).noquote() << prettyOperationDescription() << "START";

    writeToPort();
}

void ProtobufSession::writeToPort()
{
    if(!m_currentOperation || !m_plugin) {
       return;
    }

    bool success;

    do {
        const auto &buf = m_currentOperation->encodeRequest(m_plugin);
        const auto bytesWritten = m_serialPort->write(buf);

        success = bytesWritten >= 0;

        if(!success) {
            break;
        } else if(bytesWritten != buf.size()) {
            // TODO: Check for full system serial buffer
            qCCritical(LOG_SESSION) << "Serial buffer overflow";
            break;
        }

    } while(m_currentOperation->hasMoreData());

    success &= m_serialPort->flush();

    if(!success) {
        setError(BackendError::SerialError, m_serialPort->errorString());
        stopSession();
        return;
    }
}

void ProtobufSession::doStopSession()
{
    qCInfo(LOG_SESSION) << "Stopping RPC session...";

    clearOperationQueue();

    if(m_serialPort) {
        m_serialPort->close();
        m_serialPort->deleteLater();
    }

    unloadProtobufPlugin();

    qCInfo(LOG_SESSION) << "RPC session stopped successfully.";

    m_sessionState = Stopped;
    emit sessionStatusChanged();
}

void ProtobufSession::onCurrentOperationFinished()
{
    if(m_currentOperation->isError()) {
        qCCritical(LOG_SESSION).noquote() << prettyOperationDescription() << "ERROR:" << m_currentOperation->errorString();
    } else {
        qCInfo(LOG_SESSION).noquote() << prettyOperationDescription() << "SUCCESS";
    }

    m_currentOperation->deleteLater();
    m_currentOperation = nullptr;

    QTimer::singleShot(0, this, &ProtobufSession::processQueue);
}

bool ProtobufSession::loadProtobufPlugin()
{
    m_loader->setFileName(protobufPluginPath());

    if(!(m_plugin = qobject_cast<ProtobufPluginInterface*>(m_loader->instance()))) {
        qCCritical(LOG_SESSION) << "Failed to load protobuf plugin:" << m_loader->errorString();
    } else {
        m_plugin->setMinorVersion(m_versionMinor);
    }

    return m_plugin;
}

void ProtobufSession::unloadProtobufPlugin()
{
    m_plugin = nullptr;

    if(!m_loader->isLoaded()) {
        return;
    }

    qCDebug(LOG_SESSION) << "Attempting to unload protobuf plugin...";

    if(!m_loader->unload()) {
        qCDebug(LOG_SESSION) << "Cannot unload protobuf plugin. This is probably OK.";
    } else {
        qCDebug(LOG_SESSION) << "Unloaded protobuf plugin.";
    }
}

void ProtobufSession::stopEarly(BackendError::ErrorType error, const QString &errorString)
{
    m_sessionState = Stopped;
    setError(error, errorString);
    emit sessionStatusChanged();
}

uint32_t ProtobufSession::getAndIncrementCounter()
{
    // Skip 0, it is reserved for broadcast messages
    do {
        ++m_counter;
    } while(m_counter == 0);

    return m_counter;
}

void ProtobufSession::clearOperationQueue()
{
    while(!m_queue.isEmpty()) {
        m_queue.dequeue()->deleteLater();
    }

    if(m_currentOperation) {
        m_currentOperation->abort(QStringLiteral("RPC session was stopped with operations still running"));
    }
}

const QString ProtobufSession::protobufPluginPath() const
{
#if defined(Q_OS_WINDOWS)
    return QStringLiteral("flipperproto%1.dll").arg(m_versionMajor);
#elif defined(Q_OS_MAC)
    return QStringLiteral("libflipperproto%1.dylib").arg(m_versionMajor);
#elif defined(Q_OS_LINUX)
    return QStringLiteral("libflipperproto%1.so").arg(m_versionMajor);
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

    if(m_sessionState == Idle) {
        m_sessionState = Running;
        QTimer::singleShot(0, this, &ProtobufSession::processQueue);
    }

    return operation;
}
