#pragma once

#include <QQueue>
#include <QObject>
#include <QSerialPortInfo>

#include "failable.h"

class QIODevice;
class ProtobufPluginInterface;

namespace Flipper {
namespace Zero {

class AbstractProtobufOperation;

class SystemRebootOperation;
class SystemDeviceInfoOperation;
class SystemFactoryResetOperation;

class StorageListOperation;
class StorageInfoOperation;
class StorageStatOperation;
class StorageReadOperation;
class StorageMkdirOperation;
class StorageWriteOperation;
class StorageRemoveOperation;

class GuiStartStreamOperation;
class GuiStopStreamOperation;
class GuiScreenFrameOperation;
class GuiStartVirtualDisplayOperation;
class GuiStopVirtualDisplayOperation;

class ProtobufSession : public QObject, public Failable
{
    Q_OBJECT

public:
    enum SessionState {
        Starting,
        Idle,
        Running,
        Stopping,
        Stopped
    };

    ProtobufSession(const QSerialPortInfo &serialInfo, QObject *parent = nullptr);
    ~ProtobufSession();

    SessionState sessionState() const;

    void setSerialPort(const QSerialPortInfo &serialInfo);

    void setMajorVersion(int versionMajor);
    void setMinorVersion(int versionMinor);

    // Operations
    SystemRebootOperation *rebootToOS();
    SystemRebootOperation *rebootToRecovery();
    SystemFactoryResetOperation *factoryReset();
    SystemDeviceInfoOperation *systemDeviceInfo();

    StorageListOperation *storageList(const QByteArray &path);
    StorageInfoOperation *storageInfo(const QByteArray &path);
    StorageStatOperation *storageStat(const QByteArray &path);
    StorageMkdirOperation *storageMkdir(const QByteArray &path);
    StorageRemoveOperation *storageRemove(const QByteArray &path);
    StorageReadOperation *storageRead(const QByteArray &path, QIODevice *file);
    StorageWriteOperation *storageWrite(const QByteArray &path, QIODevice *file);

    GuiStartStreamOperation *guiStartStreaming();
    GuiStopStreamOperation *guiStopStreaming();

    GuiStartVirtualDisplayOperation *guiStartVirtualDisplay(const QByteArray &screenData = QByteArray());
    GuiStopVirtualDisplayOperation *guiStopVirtualDisplay();
    GuiScreenFrameOperation *guiSendScreenFrame(const QByteArray &screenData);

signals:
    void sessionStateChanged();
    void broadcastResponseReceived(QObject *response);

public slots:
//    void start();
    void stop();

private slots:
    void onSerialPortReadyRead();
    void onSerialPortBytesWriten(qint64 nbytes);
    void onSerialPortErrorOccured();

    void processQueue();
    void writeToPort();

private:
    bool loadProtobufPlugin();
    bool unloadProtobufPlugin();

    const QString protobufPluginPath() const;
    const QString prettyOperationDescription() const;

    uint32_t getAndIncrementCounter();
    void setSessionState(SessionState newState);

    template<class T>
    T* enqueueOperation(T *operation);

    void processMatchedResponse(QObject *response);
    void processBroadcastResponse(QObject *response);
    void processUnmatchedResponse(QObject *response);
    void processErrorResponse(QObject *response);

    SessionState m_sessionState;
    QSerialPort *m_serialPort;
    QByteArray m_receivedData;

    ProtobufPluginInterface *m_plugin;
    QQueue<AbstractProtobufOperation*> m_queue;
    AbstractProtobufOperation *m_currentOperation;

    qint64 m_bytesToWrite;
    uint32_t m_counter;

    int m_versionMajor;
    int m_versionMinor;
};

}
}

