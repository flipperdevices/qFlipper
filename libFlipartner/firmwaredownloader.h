#ifndef FIRMWAREUPDATER_H
#define FIRMWAREUPDATER_H

#include <QObject>
#include <QQueue>

class QIODevice;
namespace Flipper {

class Zero;

class FirmwareDownloader : public QObject
{
    Q_OBJECT

    enum class State {
        Ready,
        WaitingForDFU,
        ExecuteRequest
    };

    struct Request {
        Flipper::Zero *device;
        QIODevice *file;
    };

public:
    FirmwareDownloader(QObject *parent = nullptr);

public slots:
    void downloadLocalFile(Flipper::Zero *device, const QString &filePath);
//    void downloadRemoteFile(Flipper::Zero *device);

    void onDeviceConnected(Flipper::Zero *device);

private slots:
    void processQueue();

private:
    void processCurrentRequest();

    State m_state;
    Request m_currentRequest;
    QQueue<Request> m_requestQueue;
};

}

#endif // FIRMWAREUPDATER_H
