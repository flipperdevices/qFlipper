#include "remotefilefetcher.h"

#include <QNetworkAccessManager>
#include <QCryptographicHash>
#include <QNetworkReply>

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

void RemoteFileFetcher::onDownloadProgress(qint64 received, qint64 total)
{
    emit progressChanged((double)total / (double)received);
}
