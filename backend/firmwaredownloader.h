#ifndef FIRMWAREUPDATER_H
#define FIRMWAREUPDATER_H

#include <QObject>
#include <QQueue>

#include "flipperupdates.h"
#include "firmwareoperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;
class FirmwareDownloader : public QObject
{
    Q_OBJECT

    enum class State {
        Ready,
        Running
    };

public:
    FirmwareDownloader(QObject *parent = nullptr);

public slots:
    void downloadLocalFile(Flipper::FlipperZero *device, const QString &filePath);
    void downloadRemoteFile(Flipper::FlipperZero *device, const Updates::FileInfo &fileInfo);

private slots:
    void processQueue();

private:
    void enqueueOperation(FirmwareOperation *op);

    State m_state;
    QQueue<FirmwareOperation*> m_operationQueue;
};

}

#endif // FIRMWAREUPDATER_H
