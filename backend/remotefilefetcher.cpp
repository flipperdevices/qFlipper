#include "remotefilefetcher.h"

#include <QNetworkAccessManager>
#include <QCryptographicHash>
#include <QNetworkReply>

#include "debug.h"

using namespace Flipper;

RemoteFileFetcher::RemoteFileFetcher(QObject *parent):
    QObject(parent),
    m_manager(new QNetworkAccessManager(this))
{}

RemoteFileFetcher::RemoteFileFetcher(const QString &remoteUrl, QIODevice *outputFile, QObject *parent):
    RemoteFileFetcher(parent)
{
    fetch(remoteUrl, outputFile);
}

RemoteFileFetcher::RemoteFileFetcher(const Flipper::Updates::FileInfo &fileInfo, QIODevice *outputFile, QObject *parent):
    RemoteFileFetcher(parent)
{
    fetch(fileInfo, outputFile);
}

bool RemoteFileFetcher::fetch(const QString &remoteUrl, QIODevice *outputFile)
{
    if(!outputFile->open(QIODevice::WriteOnly)) {
        setError(BackendError::DiskError, QStringLiteral("Failed to open file for writing: %1.").arg(outputFile->errorString()));
        return false;
    }

    auto *reply = m_manager->get(QNetworkRequest(remoteUrl));

    if(reply->error() != QNetworkReply::NoError) {
        setError(BackendError::InternetError, QStringLiteral("Network error: %1").arg(reply->errorString()));

        reply->deleteLater();
        return false;
    }

    const auto onReplyReadyRead = [=]() {
        outputFile->write(reply->readAll());
    };

    connect(reply, &QNetworkReply::finished, this, [=]() {
        // In case there was any leftover data
        onReplyReadyRead();

        outputFile->close();
        reply->deleteLater();

        if(reply->error() != QNetworkReply::NoError) {
            setError(BackendError::InternetError, QStringLiteral("Network error: %1").arg(reply->errorString()));

        } else if(!m_expectedChecksum.isEmpty()) {
            if(!outputFile->open(QIODevice::ReadOnly)) {
                setError(BackendError::DiskError, QStringLiteral("Failed to open file for reading: %1.").arg(outputFile->errorString()));
                return;
            }

            QCryptographicHash hash(QCryptographicHash::Sha256);
            hash.addData(outputFile);

            if(hash.result().toHex() != m_expectedChecksum) {
                setError(BackendError::UnknownError, QStringLiteral("File integrity check failed"));
            }

            outputFile->close();
        }

        emit finished();
    });

    connect(reply, &QNetworkReply::readyRead, this, onReplyReadyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &RemoteFileFetcher::onDownloadProgress);

    return true;
}

bool RemoteFileFetcher::fetch(const Flipper::Updates::FileInfo &fileInfo, QIODevice *outputFile)
{
    m_expectedChecksum = fileInfo.sha256();
    return fetch(fileInfo.url(), outputFile);
}

void RemoteFileFetcher::onDownloadProgress(qint64 received, qint64 total)
{
    emit progressChanged(((double)received / (double)total) * 100.0);
}
