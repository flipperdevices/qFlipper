#include "storagemd5sumoperation.h"

#include "protobufplugininterface.h"

#include "mainresponseinterface.h"
#include "storageresponseinterface.h"

using namespace Flipper;
using namespace Zero;

StorageMd5SumOperation::StorageMd5SumOperation(uint32_t id, const QByteArray &path, QObject *parent):
    AbstractStorageOperation(id, path, parent)
{}

const QString StorageMd5SumOperation::description() const
{
    return QStringLiteral("Storage Md5Sum @%1").arg(QString(path()));
}

const QByteArray StorageMd5SumOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageMd5Sum(id(), path());
}

const QByteArray &StorageMd5SumOperation::md5Sum() const
{
    return m_md5Sum;
}

bool StorageMd5SumOperation::processResponse(QObject *response)
{
    if(auto *storageMd5SumResponse = qobject_cast<StorageMd5SumResponseInterface*>(response)) {
        m_md5Sum = storageMd5SumResponse->md5Sum();
        return true;

    } else {
        return qobject_cast<EmptyResponseInterface*>(response);
    }
}
