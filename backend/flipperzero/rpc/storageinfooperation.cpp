#include "storageinfooperation.h"

#include "protobufplugininterface.h"
#include "mainresponseinterface.h"
#include "storageresponseinterface.h"

using namespace Flipper;
using namespace Zero;

StorageInfoOperation::StorageInfoOperation(uint32_t id, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path),
    m_isPresent(false),
    m_sizeFree(0),
    m_sizeTotal(0)
{}

const QString StorageInfoOperation::description() const
{
    return QStringLiteral("Storage Info @%1").arg(QString(m_path));
}

bool StorageInfoOperation::isPresent() const
{
    return m_isPresent;
}

quint64 StorageInfoOperation::sizeFree() const
{
    return m_sizeFree;
}

quint64 StorageInfoOperation::sizeTotal() const
{
    return m_sizeTotal;
}

const QByteArray StorageInfoOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageInfo(id(), m_path);
}

bool StorageInfoOperation::processResponse(QObject *response)
{
    auto *storageInfoResponse = qobject_cast<StorageInfoResponseInterface*>(response);

    if(storageInfoResponse) {
        m_sizeFree = storageInfoResponse->sizeFree();
        m_sizeTotal = storageInfoResponse->sizeTotal();
        m_isPresent = true;
    }

    // Never fail on recoverable errors
    return true;
}
