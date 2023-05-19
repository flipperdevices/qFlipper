#include "filesuploadoperation.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QFile>

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagemkdiroperation.h"
#include "flipperzero/rpc/storagewriteoperation.h"

using namespace Flipper;
using namespace Zero;

FilesUploadOperation::FilesUploadOperation(ProtobufSession *rpc, DeviceState *deviceState, const QList<QUrl> &fileUrls, const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_remotePath(remotePath),
    m_urlList(fileUrls),
    m_totalSize(0)
{}

const QString FilesUploadOperation::description() const
{
    const auto numFiles = m_urlList.size();
    return QStringLiteral("Upload %1 %2 @%3").arg(QString::number(numFiles), (numFiles == 1) ? "entry" : "entries", deviceState()->deviceInfo().name);
}

void FilesUploadOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(ReadingFileList);
        readFileList();

    } else if(operationState() == ReadingFileList) {
        setOperationState(WritingFiles);
        writeFiles();

    } else if(operationState() == WritingFiles) {
        finish();
    }
}

void FilesUploadOperation::readFileList()
{
    for(const auto &url: qAsConst(m_urlList)) {
        const QFileInfo fileInfo(url.adjusted(QUrl::StripTrailingSlash).toLocalFile());
        const QDir topmostDir = fileInfo.dir();

        m_fileList.append({fileInfo, topmostDir});

        if(fileInfo.isFile()) {
            m_totalSize += fileInfo.size();

        } else if(fileInfo.isDir()) {
            QDir dir(fileInfo.absoluteFilePath());
            dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
            dir.setSorting(QDir::Name | QDir::DirsFirst);

            QDirIterator it(dir, QDirIterator::Subdirectories);
            while(it.hasNext()) {
                const QFileInfo fileInfo(it.next());

                m_fileList.append({fileInfo, topmostDir});

                if(fileInfo.isFile()) {
                    m_totalSize += fileInfo.size();
                }
            }
        }
    }

    advanceOperationState();
}

void FilesUploadOperation::writeFiles()
{
    auto fileCountLeft = m_fileList.size();
    auto fileProgress = 0.0;

    for(const auto &entry: qAsConst(m_fileList)) {
        const auto &fileInfo = entry.fileInfo;
        const auto &topmostDir = entry.topmostDir;

        const auto absoluteLocalPath = fileInfo.absoluteFilePath();
        const auto relativeLocalPath = topmostDir.relativeFilePath(absoluteLocalPath);

        const auto absoluteRemotePath = m_remotePath + QByteArrayLiteral("/") + relativeLocalPath.toLocal8Bit();
        const auto sizeRatio = (double)fileInfo.size() / m_totalSize;
        const auto isLastEntry = (--fileCountLeft == 0);

        if(fileInfo.isFile()) {
            auto *file = new QFile(absoluteLocalPath, this);
            auto *operation = rpc()->storageWrite(absoluteRemotePath, file);

            connect(operation, &AbstractOperation::progressChanged, this, [=]() {
                setProgress(fileProgress + operation->progress() * sizeRatio);
            });

            connect(operation, &AbstractOperation::finished, this, [=]() {
                if(operation->isError()) {
                    finishWithError(operation->error(), operation->errorString());
                } else if(isLastEntry) {
                    advanceOperationState();
                }
            });

        } else if(fileInfo.isDir()) {
            auto *operation = rpc()->storageMkdir(absoluteRemotePath);
            connect(operation, &AbstractOperation::finished, this, [=]() {
                if(operation->isError()) {
                    finishWithError(operation->error(), operation->errorString());
                } else if(isLastEntry) {
                    advanceOperationState();
                }
            });
        }

        fileProgress += 100.0 * sizeRatio;
    }
}
