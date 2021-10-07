#include "remotefilefetcher.h"

#include <QNetworkAccessManager>
#include <QCryptographicHash>
#include <QNetworkReply>

#include "macros.h"

using namespace Flipper;

RemoteFileFetcher::RemoteFileFetcher(QObject *parent):
    QObject(parent),
    m_manager(new QNetworkAccessManager(this))
{}

RemoteFileFetcher::~RemoteFileFetcher()
{
    // TODO: correct destruction in all cases
}

bool RemoteFileFetcher::fetch(const QString &remoteUrl, QIODevice *outputFile)
{
    if(!outputFile->open(QIODevice::ReadWrite)) {
        setError(QStringLiteral("Failed to open file for writing: %1.").arg(outputFile->errorString()));
        return false;
    }

    auto *reply = m_manager->get(QNetworkRequest(remoteUrl));

    if(reply->error() != QNetworkReply::NoError) {
        setError(QStringLiteral("Network error: %1").arg(reply->errorString()));

        reply->deleteLater();
        return false;
    }

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if(reply->error() != QNetworkReply::NoError) {
            setError(QStringLiteral("Network error: %1").arg(reply->errorString()));

        } else if(!m_expectedChecksum.isEmpty()) {
            outputFile->seek(0);

            QCryptographicHash hash(QCryptographicHash::Sha256);
            hash.addData(outputFile);

            if(hash.result().toHex() != m_expectedChecksum) {
                setError(QStringLiteral("File integrity check failed"));
            }
        }

        outputFile->close();

        // TODO: make this signal param-less
        emit finished(QByteArray());
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::readyRead, this, [=]() {
        outputFile->write(reply->readAll());
    });

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
