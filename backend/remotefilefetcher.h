#ifndef FILEFETCHER_H
#define FILEFETCHER_H

#include <QObject>

#include "failable.h"
#include "flipperupdates.h"

class QNetworkAccessManager;

class RemoteFileFetcher : public QObject, public Failable
{
    Q_OBJECT

public:
    RemoteFileFetcher(QObject *parent = nullptr);
    virtual ~RemoteFileFetcher();

    bool fetch(const QString &remoteUrl, QIODevice *outputFile);
    bool fetch(const Flipper::Updates::FileInfo &fileInfo, QIODevice *outputFile);

signals:
    void progressChanged(double);
    void finished(const QByteArray&);

private slots:
    void onDownloadProgress(qint64 received, qint64 total);

private:
    QNetworkAccessManager *m_manager;
    QByteArray m_expectedChecksum;
};

#endif // FILEFETCHER_H
