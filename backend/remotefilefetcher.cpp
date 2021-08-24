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

bool RemoteFileFetcher::fetch(const QString &remoteUrl)
{
    auto *reply = m_manager->get(QNetworkRequest(remoteUrl));

    if(reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return false;
    }

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data;

        if(reply->error() == QNetworkReply::NoError) {
            data = reply->readAll();
        }

        emit finished(data);
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::downloadProgress, this, &RemoteFileFetcher::onDownloadProgress);

    return true;
}

bool RemoteFileFetcher::fetch(const Updates::FileInfo &fileInfo)
{
    auto *reply = m_manager->get(QNetworkRequest(fileInfo.url()));

    if(reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return false;
    }

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data;

        if(reply->error() == QNetworkReply::NoError) {
            data = reply->readAll();

            QCryptographicHash hash(QCryptographicHash::Sha256);
            hash.addData(data);

            if(hash.result().toHex() != fileInfo.sha256()) {
                data.clear();
            }
        }

        emit finished(data);
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::downloadProgress, this, &RemoteFileFetcher::onDownloadProgress);

    return true;
}

bool RemoteFileFetcher::fetch(const Flipper::Updates::FileInfo &fileInfo, QIODevice *outputFile)
{
    auto *reply = m_manager->get(QNetworkRequest(fileInfo.url()));

    if(reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return false;
    }

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if(reply->error() == QNetworkReply::NoError) {
            outputFile->seek(0);

            QCryptographicHash hash(QCryptographicHash::Sha256);
            hash.addData(outputFile);

            if(hash.result().toHex() != fileInfo.sha256()) {
                error_msg("SHA256 sum does not match");
            } else {
                outputFile->seek(0);
            }
        }

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

void RemoteFileFetcher::onDownloadProgress(qint64 received, qint64 total)
{
    emit progressChanged(((double)received / (double)total) * 100.0);
}
