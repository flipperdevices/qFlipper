#include "storagereadoperation.h"

#include "protobufplugininterface.h"
#include "storageresponseinterface.h"

using namespace Flipper;
using namespace Zero;

StorageReadOperation::StorageReadOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path),
    m_file(file)
{
    connect(this, &AbstractOperation::finished, m_file, [=]() {
        m_file->close();
    });
}

const QString StorageReadOperation::description() const
{
    return QStringLiteral("Storage Read @%1").arg(QString(m_path));
}

const QByteArray StorageReadOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageRead(id(), m_path);
}

bool StorageReadOperation::begin()
{
    const auto success = m_file->open(QIODevice::WriteOnly);

    if(!success) {
        setError(BackendError::DiskError, QStringLiteral("Failed to open file for reading: %1").arg(m_file->errorString()));
    }

    return success;
}

bool StorageReadOperation::processResponse(QObject *response)
{
    auto *storageReadResponse = qobject_cast<StorageReadResponseInterface*>(response);

    if(!storageReadResponse) {
        return false;
    } else if(storageReadResponse->hasFile()) {
        return m_file->write(storageReadResponse->file().data);
    }

    return true;
}
