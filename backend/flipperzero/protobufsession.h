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
class SystemUpdateOperation;

class StorageListOperation;
class StorageInfoOperation;
class StorageStatOperation;
class StorageReadOperation;
class StorageMkdirOperation;
class StorageWriteOperation;
class StorageRenameOperation;
class StorageRemoveOperation;

class GuiStartScreenStreamOperation;
class GuiStopScreenStreamOperation;
class GuiScreenFrameOperation;
class GuiSendInputOperation;
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

    ProtobufPluginInterface *pluginInstance() const;

    bool isSessionUp() const;

    void setSerialPort(const QSerialPortInfo &portInfo);

    void setMajorVersion(int versionMajor);
    void setMinorVersion(int versionMinor);

    // Operations
    SystemRebootOperation *rebootToOS();
    SystemRebootOperation *rebootToRecovery();
    SystemRebootOperation *rebootToUpdater();
    SystemGetDateTimeOperation *getDateTime();
    SystemSetDateTimeOperation *setDateTime(const QDateTime &dateTime);
    SystemFactoryResetOperation *factoryReset();
    SystemDeviceInfoOperation *systemDeviceInfo();
    SystemUpdateOperation *systemUpdate(const QByteArray &manifestPath);

    StorageListOperation *storageList(const QByteArray &path);
    StorageInfoOperation *storageInfo(const QByteArray &path);
    StorageStatOperation *storageStat(const QByteArray &path);
    StorageMkdirOperation *storageMkdir(const QByteArray &path);
    StorageRenameOperation *storageRename(const QByteArray &oldPath, const QByteArray &newPath);
    StorageRemoveOperation *storageRemove(const QByteArray &path, bool recursive = false);
    StorageReadOperation *storageRead(const QByteArray &path, QIODevice *file);
    StorageWriteOperation *storageWrite(const QByteArray &path, QIODevice *file);

    GuiStartScreenStreamOperation *guiStartScreenStream();
    GuiStopScreenStreamOperation *guiStopScreenStream();
    GuiStartVirtualDisplayOperation *guiStartVirtualDisplay(const QByteArray &screenData = QByteArray());
    GuiStopVirtualDisplayOperation *guiStopVirtualDisplay();
    GuiSendInputOperation *guiSendInput(int key, int type);
    GuiScreenFrameOperation *guiSendScreenFrame(const QByteArray &screenData);

signals:
    void sessionStateChanged();
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
    static const QString protobufPluginFileName(int versionMajor);
    static QVector<int> supportedProtobufVersions();

    void setSessionState(SessionState newState);

    bool loadProtobufPlugin();
    void unloadProtobufPlugin();

    void stopEarly(BackendError::ErrorType error, const QString &errorString);

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

