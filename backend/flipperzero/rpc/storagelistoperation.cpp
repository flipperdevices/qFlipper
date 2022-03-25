#include "storagelistoperation.h"

#include "protobufplugininterface.h"

#include "mainresponseinterface.h"
#include "storageresponseinterface.h"

using namespace Flipper;
using namespace Zero;

StorageListOperation::StorageListOperation(uint32_t id, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path),
    m_hasPath(false)
{}

const QString StorageListOperation::description() const
{
    return QStringLiteral("Storage List @%1").arg(QString(m_path));
}

const FileInfoList &StorageListOperation::files() const
{
    return m_result;
}

bool StorageListOperation::hasPath() const
{
    return m_hasPath;
}

const QByteArray StorageListOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageList(id(), m_path);
}

bool StorageListOperation::processResponse(QObject *response)
{
    auto *listResponse = qobject_cast<StorageListResponseInterface*>(response);

    m_hasPath = listResponse;

    if(!m_hasPath) {
        return qobject_cast<EmptyResponseInterface*>(response);
    }

    const auto &files = listResponse->files();

    for(const auto &file : files) {
        m_result.append({
            file.name, m_path == QByteArrayLiteral("/") ? m_path + file.name : m_path + QByteArrayLiteral("/") + file.name,
            file.type == StorageFile::Directory ? FileType::Directory : FileType::RegularFile,
            (qint64)file.size // TODO: see if a negative value is actually used anywhere
        });
    }

    return true;
}
