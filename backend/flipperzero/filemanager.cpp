#include "filemanager.h"

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QLoggingCategory>

#include "flipperzero.h"
#include "protobufsession.h"
#include "utilityinterface.h"

#include "flipperzero/devicestate.h"

#include "rpc/storagelistoperation.h"
#include "rpc/storagereadoperation.h"
#include "rpc/storagewriteoperation.h"
#include "rpc/storagemkdiroperation.h"
#include "rpc/storageremoveoperation.h"
#include "rpc/storagerenameoperation.h"

#include "utility/directoryuploadoperation.h"
#include "utility/directorydownloadoperation.h"

#include "preferences.h"

Q_LOGGING_CATEGORY(LOG_FILEMGR, "FILEMGR")

using namespace Flipper;
using namespace Zero;

FileManager::FileManager(QObject *parent):
    QAbstractListModel(parent),
    m_device(nullptr),
    m_busyTimer(new QTimer(this)),
    m_isBusy(false),
    m_newDirectoryIndex(-1)
{
    m_busyTimer->setSingleShot(true);
    connect(m_busyTimer, &QTimer::timeout, this, &FileManager::onBusyTimerTimeout);
}

void FileManager::setDevice(FlipperZero *device)
{
    if(device == m_device) {
        return;
    }

    m_device = device;
    reset();
}

void FileManager::reset()
{
    m_forwardHistory.clear();
    m_history = QStringList{QString()};
    setModelData(FileInfoList());
}

void FileManager::refresh()
{
    listCurrentPath();
}

void FileManager::cd(const QString &dirName)
{
    pushd(dirName);
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
}

void FileManager::popd()
{
    if(m_history.size() == 1) {
        return;
    }

    m_forwardHistory.append(m_history.takeLast());
}

void FileManager::historyForward()
{
    if(m_forwardHistory.isEmpty()) {
        return;
    }

    const auto dirName = m_forwardHistory.last();

    pushd(dirName);
    listCurrentPath();
}

void FileManager::historyBack()
{
    popd();
    listCurrentPath();
}

void FileManager::rename(const QString &oldName, const QString &newName)
{
    registerOperation(m_device->rpc()->storageRename(remoteFilePath(oldName), remoteFilePath(newName)));
}

void FileManager::remove(const QString &fileName, bool recursive)
{
    registerOperation(m_device->rpc()->storageRemove(remoteFilePath(fileName), recursive));
}

void FileManager::beginMkDir()
{
    beginInsertRows(QModelIndex(), m_modelData.size(), m_modelData.size());

    FileInfo newDir;
    newDir.type = FileType::Directory;
    newDir.name = QByteArrayLiteral("New Folder");

    m_modelData.append(newDir);

    endInsertRows();
    setNewDirectoryIndex(m_modelData.size() - 1);
}

void FileManager::commitMkDir(const QString &dirName)
{
    setNewDirectoryIndex(-1);
    registerOperation(m_device->rpc()->storageMkdir(remoteFilePath(dirName)));
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

void FileManager::uploadTo(const QString &remoteDirName, const QList<QUrl> &urlList)
{
    pushd(remoteDirName);
    upload(urlList);
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

bool FileManager::isRoot() const
{
    return currentPath() == QStringLiteral("/");
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

int FileManager::newDirectoryIndex() const
{
    return m_newDirectoryIndex;
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

void FileManager::setNewDirectoryIndex(int newIndex)
{
    if(newIndex == m_newDirectoryIndex) {
        return;
    }

    m_newDirectoryIndex = newIndex;
    emit newDirectoryIndexChanged();
}

void FileManager::listCurrentPath()
{
    if(!m_device) {
        return;
    }

    auto *operation = m_device->rpc()->storageList(currentPath().toLocal8Bit());

    connect(operation, &AbstractOperation::finished, this, [=]() {
        emit currentPathChanged();

        if(operation->isError()) {
            //TODO: Error handling
        } else {
            setModelData(operation->files());
            emit refreshed();
        }
    });
}

void FileManager::uploadFile(const QFileInfo &info)
{
    auto *file = new QFile(info.absoluteFilePath(), this);
    auto *operation = m_device->rpc()->storageWrite(remoteFilePath(info.fileName()), file);

    connect(operation, &AbstractOperation::finished, file, &QObject::deleteLater);
    registerOperation(operation);
}

void FileManager::uploadDirectory(const QFileInfo &info)
{
    registerOperation(m_device->utility()->uploadDirectory(info.absoluteFilePath(), currentPath().toLocal8Bit()));
}

void FileManager::downloadFile(const QByteArray &remoteFileName, const QString &localFileName)
{
    auto *file = new QFile(localFileName, this);
    auto *operation = m_device->rpc()->storageRead(remoteFilePath(remoteFileName), file);

    connect(operation, &AbstractOperation::finished, file, &QObject::deleteLater);
    registerOperation(operation);
}

void FileManager::downloadDirectory(const QByteArray &remoteDirName, const QString &localDirName)
{
    registerOperation(m_device->utility()->downloadDirectory(localDirName, remoteFilePath(remoteDirName)));
}

void FileManager::setModelData(const FileInfoList &newData)
{
    beginResetModel();

    m_modelData = newData;

    if(isRoot()) {
        const auto hasSDCard = m_device->deviceState()->deviceInfo().storage.isExternalPresent;

        m_modelData.erase(std::remove_if(m_modelData.begin(), m_modelData.end(), [hasSDCard](const FileInfo &arg) {
            return arg.absolutePath != QStringLiteral("/int") && (!hasSDCard || arg.absolutePath != QStringLiteral("/ext"));
        }), m_modelData.end());

    } else {

        if(!globalPrefs->showHiddenFiles()) {
            m_modelData.erase(std::remove_if(m_modelData.begin(), m_modelData.end(), [](const FileInfo &arg) {
                return arg.name.startsWith('.');
            }), m_modelData.end());
        }

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

void FileManager::registerOperation(AbstractOperation *operation)
{
    setBusy(true);
    m_device->deviceState()->setProgress(-1.0);

    connect(operation, &AbstractOperation::progressChanged, m_device, [=]() {
        m_device->deviceState()->setProgress(operation->progress());
    });

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            //TODO: error handling
        } else {
            listCurrentPath();
        }

        setBusy(false);
    });
}

const QByteArray FileManager::remoteFilePath(const QString &fileName) const
{
    const auto isRoot = currentPath() == QStringLiteral("/");
    return QStringLiteral("%1/%2").arg(currentPath(), fileName).mid(isRoot ? 1 : 0).toLocal8Bit();
}
