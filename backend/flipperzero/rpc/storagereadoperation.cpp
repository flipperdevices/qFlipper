#include "storagereadoperation.h"

#include <QIODevice>

#include "protobufplugininterface.h"
#include "storageresponseinterface.h"
#include "mainresponseinterface.h"

using namespace Flipper;
using namespace Zero;

StorageReadOperation::StorageReadOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path),
    m_file(file),
    m_subRequest(NoRequest),
    m_fileSizeTotal(0),
    m_fileSizeReceived(0)
{
    connect(this, &AbstractOperation::finished, m_file, [=]() {
        m_file->close();
    });
}

const QString StorageReadOperation::description() const
{
    return QStringLiteral("Storage Read @%1").arg(QString(m_path));
}

bool StorageReadOperation::hasMoreData() const
{
    return m_subRequest != StorageRead;
}

// Custom feedResponse() implementation to accommodate the stat request
void StorageReadOperation::feedResponse(QObject *response)
{
    auto *mainResponse = qobject_cast<MainResponseInterface*>(response);

    if(mainResponse->isError()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with error: %1").arg(mainResponse->errorString()));
    } else if(!processResponse(response)) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Operation finished with error: %1").arg(mainResponse->errorString()));
    } else if(qobject_cast<StorageReadResponseInterface*>(response) && !mainResponse->hasNext()) {
        finish();
    } else {
        startTimeout();
    }
}

const QByteArray StorageReadOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    if(m_subRequest == NoRequest) {
        m_subRequest = StorageStat;
        return encoder->storageStat(id(), m_path);

    } else if(m_subRequest == StorageStat) {
        m_subRequest = StorageRead;
        return encoder->storageRead(id(), m_path);

    } else {
        return QByteArray();
    }
}

bool StorageReadOperation::begin()
{
    const auto success = m_file->open(QIODevice::WriteOnly);

    if(!success) {
        setError(BackendError::DiskError, QStringLiteral("Failed to open file for writing: %1").arg(m_file->errorString()));
    }

    return success;
}

bool StorageReadOperation::processResponse(QObject *response)
{
    if(auto *storageReadResponse = qobject_cast<StorageReadResponseInterface*>(response)) {
        if(storageReadResponse->hasFile()) {
            const auto &data = storageReadResponse->file().data;

            m_fileSizeReceived += data.size();
            setProgress(m_fileSizeReceived * 100.0 / m_fileSizeTotal);

            return m_file->write(data) == data.size();
        }

    } else if(auto *storageStatResponse = qobject_cast<StorageStatResponseInterface*>(response)) {
        if(storageStatResponse->hasFile()) {
            m_fileSizeTotal = storageStatResponse->file().size;
            return true;
        }
    }

    return false;
}
