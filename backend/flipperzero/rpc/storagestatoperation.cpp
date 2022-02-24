#include "storagestatoperation.h"

#include "mainresponseinterface.h"
#include "protobufplugininterface.h"
#include "storageresponseinterface.h"

using namespace Flipper;
using namespace Zero;

StorageStatOperation::StorageStatOperation(uint32_t id, const QByteArray &fileName, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_fileName(fileName),
    m_hasFile(false),
    m_size(0),
    m_type(FileType::Invalid)
{}

const QString StorageStatOperation::description() const
{
    return QStringLiteral("Storage Stat @%1").arg(QString(m_fileName));
}

const QByteArray &StorageStatOperation::fileName() const
{
    return m_fileName;
}

bool StorageStatOperation::hasFile() const
{
    return m_hasFile;
}

quint64 StorageStatOperation::size() const
{
    return m_size;
}

StorageStatOperation::FileType StorageStatOperation::type() const
{
    return m_type;
}

const QByteArray StorageStatOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    return encoder->storageStat(id(), m_fileName);
}

bool StorageStatOperation::processResponse(QObject *response)
{
    auto *statResponse = qobject_cast<StorageStatResponseInterface*>(response);

    if(statResponse) {
        m_hasFile = statResponse->hasFile();

        if(m_hasFile) {
            m_size = statResponse->file().size;
            m_type = (FileType)statResponse->file().type;
        }
    }

    return true;
}
