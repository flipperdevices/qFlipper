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
        WaitingForInfo,
        WaitingForDFU,
        ExecuteRequest
    };

    struct Request {
        QString serialNumber;
        QIODevice *file;
    };

public:
    FirmwareUpdater(QObject *parent = nullptr);

signals:
    void deviceInfoRequested(const QString &serialNumber);

public slots:
    void requestLocalFlash(const QString &serialNumber, const QString &filePath);
    void onDeviceConnected(const FlipperInfo &info);
    void onDeviceFound(const FlipperInfo &info);

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
