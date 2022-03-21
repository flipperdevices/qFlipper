#include "storagelistoperation.h"

#include "protobufplugininterface.h"
#include "storageresponseinterface.h"

using namespace Flipper;
using namespace Zero;

StorageListOperation::StorageListOperation(uint32_t id, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path)
{}

const QString StorageListOperation::description() const
{
    return QStringLiteral("Storage List @%1").arg(QString(m_path));
}

const FileInfoList &StorageListOperation::files() const
{
    return m_result;
}

const QByteArray StorageListOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageList(id(), m_path);
}

bool StorageListOperation::processResponse(QObject *response)
{
    auto *listResponse = qobject_cast<StorageListResponseInterface*>(response);

    if(!listResponse) {
        return false;
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
