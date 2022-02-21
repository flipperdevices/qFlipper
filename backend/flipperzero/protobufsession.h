#pragma once

#include <QQueue>
#include <QObject>
#include <QSerialPortInfo>

#include "failable.h"

class QIODevice;
class QPluginLoader;
class ProtobufPluginInterface;

namespace Flipper {
namespace Zero {

class AbstractProtobufOperation;

class SystemRebootOperation;
class SystemDeviceInfoOperation;
class SystemGetDateTimeOperation;
class SystemSetDateTimeOperation;
class SystemFactoryResetOperation;

class StorageListOperation;
class StorageInfoOperation;
class StorageStatOperation;
class StorageReadOperation;
class StorageMkdirOperation;
class StorageWriteOperation;
class StorageRemoveOperation;

class GuiStartScreenStreamOperation;
class GuiStopScreenStreamOperation;
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
        Stopped
    };

    ProtobufSession(const QSerialPortInfo &portInfo, QObject *parent = nullptr);
    ~ProtobufSession();

    bool isSessionUp() const;

    void setSerialPort(const QSerialPortInfo &portInfo);

    void setMajorVersion(int versionMajor);
    void setMinorVersion(int versionMinor);

    // Operations
    SystemRebootOperation *rebootToOS();
    SystemRebootOperation *rebootToRecovery();
    SystemGetDateTimeOperation *getDateTime();
    SystemSetDateTimeOperation *setDateTime(const QDateTime &dateTime);
    SystemFactoryResetOperation *factoryReset();
    SystemDeviceInfoOperation *systemDeviceInfo();

    StorageListOperation *storageList(const QByteArray &path);
    StorageInfoOperation *storageInfo(const QByteArray &path);
    StorageStatOperation *storageStat(const QByteArray &path);
    StorageMkdirOperation *storageMkdir(const QByteArray &path);
    StorageRemoveOperation *storageRemove(const QByteArray &path);
    StorageReadOperation *storageRead(const QByteArray &path, QIODevice *file);
    StorageWriteOperation *storageWrite(const QByteArray &path, QIODevice *file);

    GuiStartScreenStreamOperation *guiStartScreenStream();
    GuiStopScreenStreamOperation *guiStopScreenStream();
    GuiStartVirtualDisplayOperation *guiStartVirtualDisplay(const QByteArray &screenData = QByteArray());
    GuiStopVirtualDisplayOperation *guiStopVirtualDisplay();
    GuiScreenFrameOperation *guiSendScreenFrame(const QByteArray &screenData);

signals:
    void sessionStatusChanged();
    void broadcastResponseReceived(QObject *response);

public slots:
    void startSession();
    void stopSession();

private slots:
    void onSerialPortReadyRead();
    void onSerialPortBytesWriten(qint64 nbytes);
    void onSerialPortErrorOccured();

    void processQueue();
    void writeToPort();
    void doStopSession();

    void onCurrentOperationFinished();

private:
    bool loadProtobufPlugin();
    bool unloadProtobufPlugin();

    void stopEarly(BackendError::ErrorType error, const QString &errorString);

    const QString protobufPluginPath() const;
    const QString prettyOperationDescription() const;

    uint32_t getAndIncrementCounter();

    template<class T>
    T* enqueueOperation(T *operation);
    void clearOperationQueue();

    void processMatchedResponse(QObject *response);
    void processBroadcastResponse(QObject *response);
    void processUnmatchedResponse(QObject *response);
    void processErrorResponse(QObject *response);

    SessionState m_sessionState;
    QSerialPortInfo m_portInfo;
    QSerialPort *m_serialPort;
    QByteArray m_receivedData;

    QPluginLoader *m_loader;
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

