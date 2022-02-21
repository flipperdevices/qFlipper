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
//    setTimeout(30000);
}

const QString StorageWriteOperation::description() const
{
    return QStringLiteral("Storage Write @%1").arg(QString(m_path));
}

bool StorageWriteOperation::hasNext() const
{
    return m_file->bytesAvailable() > CHUNK_SIZE;
}

const QByteArray StorageWriteOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageWrite(id(), m_path, m_file->read(CHUNK_SIZE), hasNext());
}
