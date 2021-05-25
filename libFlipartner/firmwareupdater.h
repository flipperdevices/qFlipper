#ifndef FIRMWAREUPDATER_H
#define FIRMWAREUPDATER_H

#include <QScopedPointer>
#include <QObject>
#include <QQueue>

#include "flipperinfo.h"

class QIODevice;
class FirmwareUpdater : public QObject
{
    Q_OBJECT

    enum class State {
        Ready,
        WaitingForDFU,
        ExecuteRequest
    };

    struct Request {
        FlipperInfo info;
        QIODevice *file;
    };

public:
    FirmwareUpdater(QObject *parent = nullptr);

signals:
    void deviceStatusChanged(const FlipperInfo &info);

public slots:
    void requestLocalUpdate(const FlipperInfo &info, const QString &filePath);
    void onDeviceConnected(const FlipperInfo &info);

private slots:
    void processQueue();

private:
    void downloadFirmware(const FlipperInfo &info, QIODevice *file);
    void resetToDFU(const FlipperInfo &info);

    State m_state;
    Request m_currentRequest;
    QQueue<Request> m_requestQueue;
};

#endif // FIRMWAREUPDATER_H
