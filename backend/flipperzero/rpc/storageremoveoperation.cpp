#include "storageremoveoperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

StorageRemoveOperation::StorageRemoveOperation(uint32_t id, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path)
{}

const QString StorageRemoveOperation::description() const
{
    return QStringLiteral("Storage Remove @%1").arg(QString(m_path));
}

const QByteArray StorageRemoveOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageRemove(id(), m_path);
}
