#include "protobufsession.h"

#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QPluginLoader>
#include <QLoggingCategory>
#include <QCoreApplication>

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
#include "rpc/storagerenameoperation.h"
#include "rpc/storagemd5sumoperation.h"

#include "rpc/systemrebootoperation.h"
#include "rpc/systemdeviceinfooperation.h"
#include "rpc/systemgetdatetimeoperation.h"
#include "rpc/systemsetdatetimeoperation.h"
#include "rpc/systemfactoryresetoperation.h"
#include "rpc/systemupdateoperation.h"
#include "rpc/systemprotobufversionoperation.h"

#include "rpc/guisendinputoperation.h"
#include "rpc/guiscreenframeoperation.h"
#include "rpc/guistartscreenstreamoperation.h"
#include "rpc/guistopscreenstreamoperation.h"
#include "rpc/guistartvirtualdisplayoperation.h"
#include "rpc/guistopvirtualdisplayoperation.h"

#include "rpc/propertygetoperation.h"

#if defined(QT_STATIC)
Q_IMPORT_PLUGIN(ProtobufPlugin)
#endif

Q_LOGGING_CATEGORY(LOG_SESSION, "RPC")

using namespace Flipper;
using namespace Zero;

ProtobufSession::ProtobufSession(const QSerialPortInfo &portInfo, QObject *parent):
    QObject(parent),
    m_sessionState(Stopped),
    m_portInfo(portInfo),
    m_serialPort(nullptr),
#if !defined(QT_STATIC)
    m_loader(new QPluginLoader(this)),
#endif
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

ProtobufPluginInterface *ProtobufSession::pluginInstance() const
{
    return m_plugin;
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

    if(m_plugin) {
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

SystemRebootOperation *ProtobufSession::rebootToUpdater()
{
    return enqueueOperation(new SystemRebootOperation(getAndIncrementCounter(), SystemRebootOperation::RebootModeUpdate, this));
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

SystemUpdateOperation *ProtobufSession::systemUpdate(const QByteArray &manifestPath)
{
    return enqueueOperation(new SystemUpdateOperation(getAndIncrementCounter(), manifestPath, this));
}

SystemProtobufVersionOperation *ProtobufSession::systemProtobufVersion()
{
    return enqueueOperation(new SystemProtobufVersionOperation(getAndIncrementCounter(), this));
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

StorageRenameOperation *ProtobufSession::storageRename(const QByteArray &oldPath, const QByteArray &newPath)
{
    return enqueueOperation(new StorageRenameOperation(getAndIncrementCounter(), oldPath, newPath, this));
}

StorageRemoveOperation *ProtobufSession::storageRemove(const QByteArray &path, bool recursive)
{
    return enqueueOperation(new StorageRemoveOperation(getAndIncrementCounter(), path, recursive, this));
}

StorageReadOperation *ProtobufSession::storageRead(const QByteArray &path, QIODevice *file)
{
    return enqueueOperation(new StorageReadOperation(getAndIncrementCounter(), path, file, this));
}

StorageWriteOperation *ProtobufSession::storageWrite(const QByteArray &path, QIODevice *file)
{
    return enqueueOperation(new StorageWriteOperation(getAndIncrementCounter(), path, file, this));
}

StorageMd5SumOperation *ProtobufSession::storageMd5Sum(const QByteArray &path)
{
    return enqueueOperation(new StorageMd5SumOperation(getAndIncrementCounter(), path, this));
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

PropertyGetOperation *ProtobufSession::propertyGet(const QByteArray &key)
{
    return enqueueOperation(new PropertyGetOperation(getAndIncrementCounter(), key, this));
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
    setSessionState(Starting);

    if(!loadProtobufPlugin()) {
        stopEarly(BackendError::UnknownError, QStringLiteral("Suitable protocol plugin is not available"));
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

        if(!m_queue.isEmpty()) {
            setSessionState(Running);
            QTimer::singleShot(0, this, &ProtobufSession::processQueue);
        } else {
            setSessionState(Idle);
        }
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
#if !defined(QT_STATIC)
    } else if(!m_loader->isLoaded()) {
        // For some weird reason the plugin can be unloaded by connecting
        // multiple devices (although the docs say it shouldn't)
        loadProtobufPlugin();
#endif
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
        setSessionState(Idle);
        return;
    }

    m_currentOperation = m_queue.dequeue();
    qCInfo(LOG_SESSION).noquote() << prettyOperationDescription() << "START";

    connect(m_currentOperation, &AbstractOperation::finished, this, &ProtobufSession::onCurrentOperationFinished);
    m_currentOperation->start();

    if(!m_currentOperation->isError()) {
        writeToPort();
    }
}

void ProtobufSession::writeToPort()
{
    if(!m_currentOperation || !m_plugin) {
        return;
#if !defined(QT_STATIC)
    } else if(!m_loader->isLoaded()) {
        // For some weird reason the plugin can be unloaded by connecting
        // multiple devices (although the docs say it shouldn't)
        loadProtobufPlugin();
#endif
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

    if(m_currentOperation) {
        m_currentOperation->abort(QStringLiteral("RPC session was stopped with operations still running"));
    }

    if(m_serialPort) {
        m_serialPort->close();
        m_serialPort->deleteLater();
    }

    unloadProtobufPlugin();

    qCInfo(LOG_SESSION) << "RPC session stopped successfully.";

    setSessionState(Stopped);
}

void ProtobufSession::onCurrentOperationFinished()
{
    if(m_currentOperation->isError()) {
        qCCritical(LOG_SESSION).noquote() << prettyOperationDescription() << "ERROR:" << m_currentOperation->errorString();

        clearOperationQueue();

    } else {
        qCInfo(LOG_SESSION).noquote() << prettyOperationDescription() << "SUCCESS";
    }

    m_currentOperation->deleteLater();
    m_currentOperation = nullptr;

    QTimer::singleShot(0, this, &ProtobufSession::processQueue);
}

void ProtobufSession::setSessionState(SessionState newState)
{
    if(m_sessionState == newState) {
        return;
    }

    m_sessionState = newState;
    emit sessionStateChanged();
}

#if !defined(QT_STATIC)
const QString ProtobufSession::protobufPluginFileName(uint32_t versionMajor)
{
#if defined(Q_OS_WINDOWS)
    return QStringLiteral("flipperproto%1.dll").arg(versionMajor);
#elif defined(Q_OS_MAC)
    return QStringLiteral("libflipperproto%1.dylib").arg(versionMajor);
#elif defined(Q_OS_LINUX)
    return QStringLiteral("libflipperproto%1.so").arg(versionMajor);
#else
#error "Unsupported OS"
#endif
}
#endif

QVector<uint32_t> ProtobufSession::supportedProtobufVersions()
{
    QVector<uint32_t> ret;

#if defined(QT_STATIC)
    const auto staticInstances = QPluginLoader::staticInstances();

    for(const auto *instance: staticInstances) {
        if(const auto *protobufPlugin = qobject_cast<const ProtobufPluginInterface*>(instance)) {
            ret.append(protobufPlugin->versionMajor());
        }
    }

    std::sort(ret.begin(), ret.end());
#else
    const auto libraryPaths = QCoreApplication::libraryPaths();

    for(uint32_t i = 0;; ++i) {
        for(const auto &path : libraryPaths) {
            const QDir libraryDir(path);

            if(libraryDir.exists(protobufPluginFileName(i))) {
                ret.append(i);
                break;
            }
        }

        if(!ret.contains(i)) {
            break;
        }
    }
#endif
    return ret;
}

bool ProtobufSession::loadProtobufPlugin()
{
    const auto supportedVersions = supportedProtobufVersions();

    if(supportedVersions.isEmpty()) {
        qCCritical(LOG_SESSION) << "Cannot find protobuf support plugins";
        return false;
    } else if(!supportedVersions.contains(m_versionMajor)) {
        qCCritical(LOG_SESSION).noquote() << "Protocol version" << m_versionMajor
                                          << "is not supported yet. Please update the application.";
        return false;
    }

#if defined(QT_STATIC)
    const auto staticInstances = QPluginLoader::staticInstances();

    for(auto *instance : staticInstances) {
        if(auto *protobufPlugin = qobject_cast<ProtobufPluginInterface*>(instance)) {
            if(protobufPlugin->versionMajor() == m_versionMajor) {
                m_plugin = protobufPlugin;
                break;
            }
        }
    }
#else
    m_loader->setFileName(protobufPluginFileName(m_versionMajor));
    m_plugin = qobject_cast<ProtobufPluginInterface*>(m_loader->instance());

    if(!m_plugin) {
        qCCritical(LOG_SESSION) << "Failed to load protobuf plugin:" << m_loader->errorString();
        return false;
    }
#endif

    m_plugin->setMinorVersion(m_versionMinor);
    return true;
}

void ProtobufSession::unloadProtobufPlugin()
{
    m_plugin = nullptr;

#if defined(QT_SHARED)
    if(!m_loader->isLoaded()) {
        return;
    }

    qCDebug(LOG_SESSION) << "Attempting to unload protobuf plugin...";

    if(!m_loader->unload()) {
        qCDebug(LOG_SESSION) << "Cannot unload protobuf plugin. This is probably OK.";
    } else {
        qCDebug(LOG_SESSION) << "Unloaded protobuf plugin.";
    }
#endif
}

void ProtobufSession::stopEarly(BackendError::ErrorType error, const QString &errorString)
{
    setError(error, errorString);
    setSessionState(Stopped);
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
        QTimer::singleShot(0, this, &ProtobufSession::processQueue);
        setSessionState(Running);
    }

    return operation;
}
