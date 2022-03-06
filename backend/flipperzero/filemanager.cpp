#include "filemanager.h"

#include <QDebug>
#include <QLoggingCategory>

#include "flipperzero.h"
#include "protobufsession.h"

#include "rpc/storagelistoperation.h"

Q_LOGGING_CATEGORY(LOG_FILEMGR, "FILEMGR")

using namespace Flipper;
using namespace Zero;

FileManager::FileManager(QObject *parent):
    QAbstractListModel(parent),
    m_device(nullptr),
    m_isBusy(false)
{}

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

void FileManager::setBusy(bool busy)
{
    if(m_isBusy == busy) {
        return;
    }

    m_isBusy = busy;
    emit isBusyChanged();
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

void FileManager::setModelData(const FileInfoList &newData)
{
    beginResetModel();

    m_modelData = newData;

    if(currentPath() == QStringLiteral("/")) {
        m_modelData.erase(std::remove_if(m_modelData.begin(), m_modelData.end(), [](const FileInfo &arg) {
            return arg.absolutePath == QStringLiteral("/any");
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
