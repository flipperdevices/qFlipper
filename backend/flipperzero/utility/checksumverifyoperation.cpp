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

ChecksumVerifyOperation::ChecksumVerifyOperation(ProtobufSession *rpc, DeviceState *deviceState, const QString &localDirectory, const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_localDirectory(localDirectory),
    m_remotePath(remotePath)
{
    m_localDirectory.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
}

const QString ChecksumVerifyOperation::description() const
{
    return QStringLiteral("Verify checksum @%1/%2").arg(QString(m_remotePath), m_localDirectory.dirName());
}

const QList<QUrl> &ChecksumVerifyOperation::result() const
{
    return m_result;
}

void ChecksumVerifyOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(VerifyingMd5Sum);
        verifyMd5Sums();

    } else if(operationState() == VerifyingMd5Sum) {
        finish();
    } else {
        finishWithError(BackendError::UnknownError, QStringLiteral("Unexpected state"));
    }
}

void ChecksumVerifyOperation::verifyMd5Sums()
{
    QDirIterator it(m_localDirectory, QDirIterator::Subdirectories);
    QFileInfoList files;

    while(it.hasNext()) {
        const QFileInfo fileInfo(it.next());
        if(fileInfo.isFile()) {
            files.append(fileInfo);
        }
    }

    const auto totalSize = std::accumulate(files.cbegin(), files.cend(), (qint64)0, [](qint64 sum, const QFileInfo &arg) {
        return sum + arg.size();
    });

    auto filesRemaining = files.size();

    setProgress(0.0);

    for(const auto &fileInfo : files) {
        const auto relativeFileName = m_localDirectory.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();
        const auto remoteAbsoluteFileName = m_remotePath + QByteArrayLiteral("/") + m_localDirectory.dirName().toLocal8Bit() + QByteArrayLiteral("/") + relativeFileName;

        const auto isLastFile = (--filesRemaining == 0);

        auto *operation = rpc()->storageMd5Sum(remoteAbsoluteFileName);

        connect(operation, &AbstractOperation::finished, this, [=]() {
            if(operation->isError()) {
                finishWithError(operation->error(), operation->errorString());
                return;
            }

            const auto checksumRemote = operation->md5Sum();

            if(checksumRemote.isEmpty()) {
                m_result.append(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
                qCDebug(CATEGORY_DEBUG).noquote().nospace() << "File does not exist: " << remoteAbsoluteFileName;

            } else {
                const auto checksumLocal = calculateMd5Sum(fileInfo);
                if(checksumRemote != checksumLocal) {
                    m_result.append(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
                    qCDebug(CATEGORY_DEBUG).noquote().nospace() << "Md5 mismatch for file " << remoteAbsoluteFileName
                                                                << ": remote: " << checksumRemote << ", local: " << checksumLocal;
                } else {
                    qCDebug(CATEGORY_DEBUG).noquote().nospace() << "File " << remoteAbsoluteFileName << " already exists and is identical";
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
