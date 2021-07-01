#ifndef FIRMWAREUPDATER_H
#define FIRMWAREUPDATER_H

#include <QObject>
#include <QQueue>

#include "flipperupdates.h"

class QIODevice;

namespace Flipper {

class FlipperZero;
class FirmwareDownloader : public QObject
{
    Q_OBJECT

    enum class State {
        Ready,
        WaitingForDFU,
        ExecuteRequest
    };

    struct Request {
        Flipper::FlipperZero *device;
        QIODevice *file;
    };

public:
    FirmwareDownloader(QObject *parent = nullptr);

public slots:
    void downloadLocalFile(Flipper::FlipperZero *device, const QString &filePath);
    void downloadRemoteFile(Flipper::FlipperZero *device, const Updates::FileInfo &fileInfo);

    void onDeviceConnected(Flipper::FlipperZero *device);

private slots:
    void processQueue();

private:
    void enqueueRequest(const Request &req);
    void processCurrentRequest();

    State m_state;
    Request m_currentRequest;
    QQueue<Request> m_requestQueue;
};

}

#endif // FIRMWAREUPDATER_H
