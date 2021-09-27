#include "firmwaredownloader.h"

#include <QUrl>
#include <QFile>
#include <QTimer>
#include <QBuffer>

#include "flipperzero/flipperzero.h"
#include "flipperzero/operations/userbackupoperation.h"
#include "flipperzero/operations/userrestoreoperation.h"
#include "flipperzero/operations/fixbootissuesoperation.h"
#include "flipperzero/operations/assetsdownloadoperation.h"
#include "flipperzero/operations/fixoptionbytesoperation.h"
#include "flipperzero/operations/firmwaredownloadoperation.h"
#include "flipperzero/operations/wirelessstackdownloadoperation.h"

#include "remotefilefetcher.h"
#include "macros.h"

using namespace Flipper;

FirmwareDownloader::FirmwareDownloader(QObject *parent):
    QObject(parent),
    m_state(State::Ready)
{}

void FirmwareDownloader::downloadLocalFile(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueOperation(new Flipper::Zero::FirmwareDownloadOperation(device, file));
}

void FirmwareDownloader::downloadRemoteFile(FlipperZero *device, const Flipper::Updates::VersionInfo &versionInfo)
{
    // TODO: Local cache on hard disk?
    const auto fileInfo = versionInfo.fileInfo(QStringLiteral("full_dfu"), device->target());

    auto *fetcher = new RemoteFileFetcher(this);
    auto *buf = new QBuffer(this);

    check_return_void(buf->open(QIODevice::ReadWrite), "Failed to create intermediate buffer.");

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        buf->seek(0);
        buf->close();

        enqueueOperation(new Flipper::Zero::FirmwareDownloadOperation(device, buf));

        fetcher->deleteLater();
    });

    device->setMessage(tr("Fetching the update file..."));
    fetcher->fetch(fileInfo, buf);
}

void FirmwareDownloader::downloadLocalFUS(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueOperation(new Flipper::Zero::WirelessStackDownloadOperation(device, file, 0x080EC000));
}

void FirmwareDownloader::downloadLocalWirelessStack(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueOperation(new Flipper::Zero::WirelessStackDownloadOperation(device, file));
}

void FirmwareDownloader::fixBootIssues(FlipperZero *device)
{
    enqueueOperation(new Flipper::Zero::FixBootIssuesOperation(device));
}

void FirmwareDownloader::fixOptionBytes(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueOperation(new Flipper::Zero::FixOptionBytesOperation(device, file));
}

void FirmwareDownloader::downloadAssets(FlipperZero *device, const QString &filePath)
{
    const auto localUrl = QUrl(filePath).toLocalFile();
    auto *file = new QFile(localUrl, this);

    enqueueOperation(new Flipper::Zero::AssetsDownloadOperation(device, file, this));
}

void FirmwareDownloader::backupUserData(FlipperZero *device, const QString &backupPath)
{
    enqueueOperation(new Flipper::Zero::UserBackupOperation(device, backupPath, this));
}

void FirmwareDownloader::restoreUserData(FlipperZero *device, const QString &backupPath)
{
    enqueueOperation(new Flipper::Zero::UserRestoreOperation(device, backupPath, this));
}

void FirmwareDownloader::processQueue()
{
    if(m_operationQueue.isEmpty()) {
        m_state = State::Ready;
        return;
    }

    auto *currentOperation = m_operationQueue.dequeue();

    connect(currentOperation, &AbstractOperation::finished, this, [=]() {
        info_msg(QStringLiteral("Operation '%1' finished with status: %2.").arg(currentOperation->description(), currentOperation->errorString()));
        currentOperation->deleteLater();

        QTimer::singleShot(0, this, &FirmwareDownloader::processQueue);
    });

    currentOperation->start();
}

void FirmwareDownloader::enqueueOperation(AbstractOperation *op)
{
    m_operationQueue.enqueue(op);

    if(m_state == State::Ready) {
        // Leave the context before calling processQueue()
        m_state = State::Running;
        QTimer::singleShot(20, this, &FirmwareDownloader::processQueue);
    }
}
