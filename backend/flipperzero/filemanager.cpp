#include "filemanager.h"

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QLoggingCategory>

#include "flipperzero.h"
#include "protobufsession.h"
#include "utilityinterface.h"

#include "rpc/storagelistoperation.h"
#include "rpc/storagereadoperation.h"
#include "rpc/storagewriteoperation.h"
#include "rpc/storageremoveoperation.h"
#include "rpc/storagerenameoperation.h"

#include "utility/directoryuploadoperation.h"
#include "utility/directorydownloadoperation.h"

Q_LOGGING_CATEGORY(LOG_FILEMGR, "FILEMGR")

using namespace Flipper;
using namespace Zero;

FileManager::FileManager(QObject *parent):
    QAbstractListModel(parent),
    m_device(nullptr),
    m_busyTimer(new QTimer(this)),
    m_isBusy(false)
{
    m_busyTimer->setSingleShot(true);
    connect(m_busyTimer, &QTimer::timeout, this, &FileManager::onBusyTimerTimeout);
}

void FileManager::setDevice(FlipperZero *device)
{
    m_device = device;
    reset();
}

void FileManager::reset()
{
    m_forwardHistory.clear();
    m_history = QStringList{QString()};
}

void FileManager::refresh()
{
    listCurrentPath();
}

void FileManager::pushd(const QString &dirName)
{
    if(!m_forwardHistory.isEmpty()) {
        if(m_forwardHistory.last() != dirName) {
            m_forwardHistory.clear();
        } else {
            m_forwardHistory.removeLast();
        }
    }

    m_history.append(dirName);
    listCurrentPath();
}

void FileManager::popd()
{
    if(m_history.size() == 1) {
        return;
    }

    m_forwardHistory.append(m_history.takeLast());
    listCurrentPath();
}

void FileManager::historyForward()
{
    if(m_forwardHistory.isEmpty()) {
        return;
    }

    const auto dirName = m_forwardHistory.last();
    pushd(dirName);
}

void FileManager::historyBack()
{
    popd();
}

void FileManager::rename(const QString &oldName, const QString &newName)
{
    auto *operation = m_device->rpc()->storageRename(remoteFilePath(oldName), remoteFilePath(newName));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            //TODO: Error handling
        } else {
            listCurrentPath();
        }
    });
}

void FileManager::remove(const QString &fileName, bool recursive)
{
    const auto filePath = QStringLiteral("%1/%2").arg(currentPath(), fileName);
    auto *operation = m_device->rpc()->storageRemove(filePath.toLocal8Bit(), recursive);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            //TODO: Error handling
        } else {
            listCurrentPath();
        }
    });
}

void FileManager::upload(const QList<QUrl> &urlList)
{
    for(const auto &url : urlList) {
        const QFileInfo info(url.toLocalFile());

        if(info.isDir()) {
            uploadDirectory(info);
        } else {
            uploadFile(info);
        }
    }
}

void FileManager::download(const QString &remoteFileName, const QUrl &localUrl, bool recursive)
{
    const auto remote = remoteFileName.toLocal8Bit();
    const auto local = localUrl.toLocalFile();

    if(recursive) {
        downloadDirectory(remote, local);
    } else {
        downloadFile(remote, local);
    }
}

bool FileManager::isBusy() const
{
    return m_isBusy;
}

bool FileManager::canGoBack() const
{
    return m_history.size() > 1;
}

bool FileManager::canGoForward() const
{
    return !m_forwardHistory.isEmpty();
}

QString FileManager::currentPath() const
{
    if(m_history.size() == 1) {
        return QStringLiteral("/");
    } else {
        return m_history.join('/');
    }
}

int FileManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_modelData.size();
}

QVariant FileManager::data(const QModelIndex &index, int role) const
{
    const auto &item = m_modelData[index.row()];

    switch(role) {
    case FileName:
        return item.name;
    case FilePath:
        return item.absolutePath;
    case FileType:
        return (int)item.type;
    case FileSize:
        return item.size;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FileManager::roleNames() const
{
    static const QHash<int, QByteArray> roles = {
        { FileName, QByteArrayLiteral("fileName") },
        { FilePath, QByteArrayLiteral("filePath") },
        { FileType, QByteArrayLiteral("fileType") },
        { FileSize, QByteArrayLiteral("fileSize") },
    };

    return roles;
}

void FileManager::onBusyTimerTimeout()
{
    m_isBusy = true;
    emit isBusyChanged();
}

void FileManager::setBusy(bool busy)
{
    // Do not mark short operations as busy to avoid visual noise
    if(busy) {
        if(!m_busyTimer->isActive()) {
            m_busyTimer->start(500);
        }

    } else {
        m_busyTimer->stop();

        if(m_isBusy) {
            m_isBusy = false;
            emit isBusyChanged();
        }
    }
}

void FileManager::listCurrentPath()
{
    if(!m_device) {
        return;
    }

    setBusy(true);

    auto *operation = m_device->rpc()->storageList(currentPath().toLocal8Bit());

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            //TODO: Error handling
        } else {
            setModelData(operation->files());
            emit currentPathChanged();
        }

        setBusy(false);
    });
}

void FileManager::uploadFile(const QFileInfo &info)
{
    setBusy(true);

    auto *file = new QFile(info.absoluteFilePath(), this);
    auto *operation = m_device->rpc()->storageWrite(remoteFilePath(info.fileName()), file);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        file->deleteLater();
        if(operation->isError()) {
            //TODO: Error handling
        } else {
            listCurrentPath();
        }

        setBusy(false);
    });
}

void FileManager::uploadDirectory(const QFileInfo &info)
{
    setBusy(true);

    auto *operation = m_device->utility()->uploadDirectory(info.absoluteFilePath(), currentPath().toLocal8Bit());

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            //TODO: Error handling
        } else {
            listCurrentPath();
        }

        setBusy(false);
    });
}

void FileManager::downloadFile(const QByteArray &remoteFileName, const QString &localFileName)
{
    setBusy(true);

    auto *file = new QFile(localFileName, this);
    auto *operation = m_device->rpc()->storageRead(remoteFilePath(remoteFileName), file);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        file->deleteLater();

        if(operation->isError()) {
            //TODO: Error handling
        } else {
            listCurrentPath();
        }

        setBusy(false);
    });
}

void FileManager::downloadDirectory(const QByteArray &remoteDirName, const QString &localDirName)
{
    setBusy(true);

    auto *operation = m_device->utility()->downloadDirectory(localDirName, remoteFilePath(remoteDirName));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            //TODO: Error handling
        } else {
            listCurrentPath();
        }

        setBusy(false);
    });
}

void FileManager::setModelData(const FileInfoList &newData)
{
    beginResetModel();

    m_modelData = newData;

    if(currentPath() == QStringLiteral("/")) {
        m_modelData.erase(std::remove_if(m_modelData.begin(), m_modelData.end(), [](const FileInfo &arg) {
            return arg.absolutePath != QStringLiteral("/ext") && arg.absolutePath != QStringLiteral("/int");
        }), m_modelData.end());

    } else {
        std::sort(m_modelData.begin(), m_modelData.end(), [](const FileInfo &a, const FileInfo &b) {
            if(a.type != b.type) {
                return a.type < b.type;
            } else {
                return a.name.toLower() < b.name.toLower();
            }
        });
    }

    endResetModel();
}

const QByteArray FileManager::remoteFilePath(const QString &fileName) const
{
    const auto isRoot = currentPath() == QStringLiteral("/");
    return QStringLiteral("%1/%2").arg(currentPath(), fileName).mid(isRoot ? 1 : 0).toLocal8Bit();
}
