#ifndef FIRMWAREDOWNLOADTASK_H
#define FIRMWAREDOWNLOADTASK_H

#include <QObject>
#include <QRunnable>

#include "flipperinfo.h"

class QIODevice;
class FirmwareDownloadTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    FirmwareDownloadTask(const FlipperInfo &info, QIODevice *file);
    ~FirmwareDownloadTask();

    void run() override;

signals:
    void finished();
    void progressChanged(int);
    void statusChanged(const QString);

private slots:
    void onProgressChanged(int progress);

private:
    FlipperInfo m_info;
    QIODevice *m_file;

    int m_prevProgress;
    int m_currentProgress;
};

#endif // FIRMWAREDOWNLOADTASK_H
