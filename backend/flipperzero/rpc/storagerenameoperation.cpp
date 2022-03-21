#include "storagerenameoperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

StorageRenameOperation::StorageRenameOperation(uint32_t id, const QByteArray &oldPath, const QByteArray &newPath, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_oldPath(oldPath),
    m_newPath(newPath)
{}

const QString StorageRenameOperation::description() const
{
    return QStringLiteral("Storage Rename @%1 -> %2").arg(QString(m_oldPath), QString(m_newPath));
}

const QByteArray StorageRenameOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageRename(id(), m_oldPath, m_newPath);
}
