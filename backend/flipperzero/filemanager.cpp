#include "filemanager.h"

#include "flipperzero.h"
#include "protobufsession.h"

#include "rpc/storagelistoperation.h"

using namespace Flipper;
using namespace Zero;

FileManager::FileManager(QObject *parent):
    QAbstractListModel(parent)
{}

void FileManager::setDevice(FlipperZero *device)
{
    m_device = device;
    cd(QStringLiteral("/"));
}

void FileManager::cd(const QString &dirName)
{
    if(!m_device) {
        return;
    }

    auto *operation = m_device->rpc()->storageList(QStringLiteral("%1/%2").arg(currentPath(), dirName).toLocal8Bit());

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            //TODO: Error handling
        } else {
            m_history.append(dirName);
            setModelData(operation->files());
        }
    });
}

QString FileManager::currentPath() const
{
    if(m_history.size() == 1) {
        return QString();
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
    Q_UNUSED(index)
    Q_UNUSED(role)

    return QVariant();
}

QHash<int, QByteArray> FileManager::roleNames() const
{
    static const QHash<int, QByteArray> roles = {
        { FileName, QByteArrayLiteral("fileName") },
        { FileType, QByteArrayLiteral("fileType") },
        { FileSize, QByteArrayLiteral("fileSize") },
    };

    return roles;
}

void FileManager::setModelData(const FileInfoList &newData)
{
    beginResetModel();
    m_modelData = newData;
    endResetModel();
}
