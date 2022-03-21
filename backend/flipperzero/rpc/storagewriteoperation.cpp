#include "storagewriteoperation.h"

#include <QIODevice>

#include "protobufplugininterface.h"

static constexpr qint64 CHUNK_SIZE = 512;

using namespace Flipper;
using namespace Zero;

StorageWriteOperation::StorageWriteOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path),
    m_file(file)
{
    // Write operations can be lenghty
    setTimeout(60000);
}

const QString StorageWriteOperation::description() const
{
    return QStringLiteral("Storage Write @%1").arg(QString(m_path));
}

bool StorageWriteOperation::hasMoreData() const
{
    return m_file->bytesAvailable();
}

const QByteArray StorageWriteOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    const auto buf = m_file->read(CHUNK_SIZE);
    const auto hasNext = m_file->bytesAvailable() > 0;
    return encoder->storageWrite(id(), m_path, buf, hasNext);
}

bool StorageWriteOperation::begin()
{
    const auto success = m_file->open(QIODevice::ReadOnly);

    if(!success) {
        setError(BackendError::DiskError, QStringLiteral("Failed to open file for reading: %1").arg(m_file->errorString()));
    }

    return success;
}
