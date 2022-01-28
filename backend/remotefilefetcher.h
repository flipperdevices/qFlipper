#pragma once

#include <QObject>

#include "failable.h"
#include "flipperupdates.h"

class QNetworkAccessManager;

class RemoteFileFetcher : public QObject, public Failable
{
    Q_OBJECT

public:
    RemoteFileFetcher(QObject *parent = nullptr);
    RemoteFileFetcher(const QString &remoteUrl, QIODevice *outputFile, QObject *parent = nullptr);
    RemoteFileFetcher(const Flipper::Updates::FileInfo &fileInfo, QIODevice *outputFile, QObject *parent = nullptr);

    bool fetch(const QString &remoteUrl, QIODevice *outputFile);
    bool fetch(const Flipper::Updates::FileInfo &fileInfo, QIODevice *outputFile);

signals:
    void progressChanged(double);
    void finished();

private slots:
    void onDownloadProgress(qint64 received, qint64 total);

private:
    QNetworkAccessManager *m_manager;
    QByteArray m_expectedChecksum;
};
