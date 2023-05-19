#include "checksumverifyoperation.h"

#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QCryptographicHash>

#include <QDebug>
#include <QLoggingCategory>

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagemd5sumoperation.h"

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

using namespace Flipper;
using namespace Zero;

ChecksumVerifyOperation::ChecksumVerifyOperation(ProtobufSession *rpc, DeviceState *deviceState,
                                                 const QList<QUrl> &urlsToCheck, const QByteArray &remoteRootPath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_remoteRootPath(remoteRootPath),
    m_urlsToCheck(urlsToCheck)
{}

const QString ChecksumVerifyOperation::description() const
{
    const auto numFiles = m_urlsToCheck.size();
    return QStringLiteral("Verify checksum of %1 %2 @%3").arg(QString::number(numFiles), (numFiles == 1) ? "item" : "items", deviceState()->deviceInfo().name);
}

const QList<QUrl> &ChecksumVerifyOperation::changedUrls() const
{
    return m_changedUrls;
}

void ChecksumVerifyOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(ReadingFileList);
        readFileList();

    } else if(operationState() == ReadingFileList) {
        setOperationState(VerifyingMd5Sum);
        verifyMd5Sums();

    } else if(operationState() == VerifyingMd5Sum) {
        finish();

    } else {
        finishWithError(BackendError::UnknownError, QStringLiteral("Unexpected state"));
    }
}

void ChecksumVerifyOperation::readFileList()
{
    for(const auto &url : qAsConst(m_urlsToCheck)) {
        const QFileInfo fileInfo(url.adjusted(QUrl::StripTrailingSlash).toLocalFile());
        const QDir topmostDir = fileInfo.dir();

        if(fileInfo.isFile()) {
            m_flatFileList.append({fileInfo, topmostDir});

        } else if(fileInfo.isDir()) {
            QDir dir(fileInfo.absoluteFilePath());
            dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
            dir.setSorting(QDir::Name | QDir::DirsFirst);

            QDirIterator it(dir, QDirIterator::Subdirectories);
            while(it.hasNext()) {
                const QFileInfo fileInfo(it.next());
                if(fileInfo.isFile()) {
                    m_flatFileList.append({fileInfo, topmostDir});
                }
            }
        }
    }

    advanceOperationState();
}

void ChecksumVerifyOperation::verifyMd5Sums()
{
    const auto totalSize = std::accumulate(m_flatFileList.cbegin(), m_flatFileList.cend(), (qint64)0,
                                           [](qint64 sum, const FileListElement &arg) {
        return sum + arg.fileInfo.size();
    });

    auto filesRemaining = m_flatFileList.size();

    setProgress(0.0);

    for(const auto &entry : qAsConst(m_flatFileList)) {
        const auto &fileInfo = entry.fileInfo;
        const auto &topmostDir = entry.topmostDir;

        const auto absoluteLocalFilePath = fileInfo.absoluteFilePath();
        const auto relativeLocalFilePath = topmostDir.relativeFilePath(absoluteLocalFilePath);

        const auto absoluteRemoteFilePath = m_remoteRootPath + QByteArrayLiteral("/") + relativeLocalFilePath.toLocal8Bit();
        const auto isLastFile = (--filesRemaining == 0);

        auto *operation = rpc()->storageMd5Sum(absoluteRemoteFilePath);

        connect(operation, &AbstractOperation::finished, this, [=]() {
            if(operation->isError()) {
                finishWithError(operation->error(), operation->errorString());
                return;
            }

            const auto checksumRemote = operation->md5Sum();

            if(checksumRemote.isEmpty()) {
                m_changedUrls.append(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
                qCDebug(CATEGORY_DEBUG) << "File does not exist:" << absoluteRemoteFilePath;

            } else {
                const auto checksumLocal = calculateMd5Sum(fileInfo);
                if(checksumRemote != checksumLocal) {
                    m_changedUrls.append(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
                    qCDebug(CATEGORY_DEBUG) << "File changed:" << absoluteRemoteFilePath
                                            << "old:" << checksumRemote << "new:" << checksumLocal;
                } else {
                    qCDebug(CATEGORY_DEBUG) << "File is identical:" << absoluteRemoteFilePath;
                }
            }

            setProgress(progress() + 100.0 * fileInfo.size() / totalSize);

            if(isLastFile) {
                advanceOperationState();
            }
        });
    }
}

const QByteArray ChecksumVerifyOperation::calculateMd5Sum(const QFileInfo &fileInfo)
{
    QFile file(fileInfo.absoluteFilePath());

    if(!file.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(&file);
    return hash.result().toHex();
}
