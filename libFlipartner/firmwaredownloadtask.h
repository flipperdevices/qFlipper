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
    void statusChanged(const FlipperInfo &info);

private slots:
    void onProgressChanged(const int operation, const double progress);

private:
    FlipperInfo m_info;
    QIODevice *m_file;
};

#endif // FIRMWAREDOWNLOADTASK_H
