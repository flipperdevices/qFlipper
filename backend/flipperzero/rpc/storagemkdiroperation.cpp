#include "storagemkdiroperation.h"

#include "protobufplugininterface.h"

using namespace Flipper;
using namespace Zero;

StorageMkdirOperation::StorageMkdirOperation(uint32_t id, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path)
{}

const QString StorageMkdirOperation::description() const
{
    return QStringLiteral("Storage MkDir @%1").arg(QString(m_path));
}

const QByteArray StorageMkdirOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageMkDir(id(), m_path);
}
