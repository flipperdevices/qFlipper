#include "storagewriteoperation.h"

#include <QIODevice>

#include "protobufplugininterface.h"
#include "statusresponseinterface.h"
#include "mainresponseinterface.h"

static constexpr qint64 CHUNK_SIZE = 512;

using namespace Flipper;
using namespace Zero;

StorageWriteOperation::StorageWriteOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractProtobufOperation(id, parent),
    m_path(path),
    m_file(file),
    m_subRequest(StorageWrite),
    m_chunksPerPing(0),
    m_chunksWritten(0)
{}

const QString StorageWriteOperation::description() const
{
    return QStringLiteral("Storage Write @%1").arg(QString(m_path));
}

bool StorageWriteOperation::hasMoreData() const
{
    return m_file->bytesAvailable() > 0;
}

// Custom feedResponse() implementation to accomodate interspersed pings
void StorageWriteOperation::feedResponse(QObject *response)
{
    auto *mainResponse = qobject_cast<MainResponseInterface*>(response);

    if(mainResponse->isError()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with error: %1").arg(mainResponse->errorString()));
    } else if(!processResponse(response)) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Operation finished with error: %1").arg(mainResponse->errorString()));
    } else if(!qobject_cast<StatusPingResponseInterface*>(response)) {
        finish();
    } else {
        startTimeout();
    }
}

const QByteArray StorageWriteOperation::encodeRequest(ProtobufPluginInterface *encoder)
{
    if(m_subRequest == StorageWrite) {
        if(++m_chunksWritten == m_chunksPerPing) {
            m_chunksWritten = 0;
            m_subRequest = StatusPing;
        }

        const auto buf = m_file->read(CHUNK_SIZE);
        const auto hasNext = m_file->bytesAvailable() > 0;
        return encoder->storageWrite(id(), m_path, buf, hasNext);

    } else if(m_subRequest == StatusPing) {
        m_subRequest = StorageWrite;
        return encoder->statusPing(id());
    }

    return QByteArray();
}

bool StorageWriteOperation::begin()
{
    const auto success = m_file->open(QIODevice::ReadOnly);

    if(!success) {
        setError(BackendError::DiskError, QStringLiteral("Failed to open file for reading: %1").arg(m_file->errorString()));
    } else if(m_file->bytesAvailable() >= CHUNK_SIZE * 100) {
        // Insert a ping for each 1% of the file size
        m_chunksPerPing = m_file->bytesAvailable() / (CHUNK_SIZE * 100);
    }

    return success;
}

bool StorageWriteOperation::processResponse(QObject *response)
{
    setProgress(m_chunksPerPing ? progress() + 1.0 : 100.0);
    return qobject_cast<StatusPingResponseInterface*>(response) ||
           qobject_cast<EmptyResponseInterface*>(response);
}
