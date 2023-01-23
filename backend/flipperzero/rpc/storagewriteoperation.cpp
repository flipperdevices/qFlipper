#include "storagewriteoperation.h"

#include <QIODevice>

#include "protobufplugininterface.h"
#include "statusresponseinterface.h"
#include "mainresponseinterface.h"

static constexpr qint64 CHUNK_SIZE = 1024;
static constexpr qint64 CHUNKS_PER_PING_CAP = 1000;

using namespace Flipper;
using namespace Zero;

StorageWriteOperation::StorageWriteOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractStorageOperation(id, path, parent),
    m_file(file),
    m_subRequest(StorageWrite),
    m_chunksPerPing(0),
    m_chunksWritten(0)
{
    connect(this, &AbstractOperation::finished, m_file, [=]() {
        m_file->close();
    });
}

const QString StorageWriteOperation::description() const
{
    return QStringLiteral("Storage Write @%1").arg(QString(path()));
}

bool StorageWriteOperation::hasMoreData() const
{
    return m_file->bytesAvailable() > 0;
}

// Custom feedResponse() implementation to accommodate interspersed pings
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
        // Never true for files smaller than 100 chunks
        // Such small files don't get progress reports at all
        if(++m_chunksWritten == m_chunksPerPing) {
            m_chunksWritten = 0;
            m_subRequest = StatusPing;
        }

        const auto buf = m_file->read(CHUNK_SIZE);
        const auto hasNext = m_file->bytesAvailable() > 0;
        return encoder->storageWrite(id(), path(), buf, hasNext);

    } else if(m_subRequest == StatusPing) {
        m_subRequest = StorageWrite;
        return encoder->statusPing(id());
    }

    return QByteArray();
}

bool StorageWriteOperation::begin()
{
    if(!m_file->open(QIODevice::ReadOnly)) {
        setError(BackendError::DiskError, QStringLiteral("Failed to open file for reading: %1").arg(m_file->errorString()));
        return false;
    }

    const auto fileSize = m_file->bytesAvailable();

    if(fileSize >= CHUNK_SIZE * 100) {
        // Insert a ping for roughly each 1% of the file size
        m_chunksPerPing = qMin<qint64>(fileSize / (CHUNK_SIZE * 100), CHUNKS_PER_PING_CAP);
        m_percentPerPing = 100.0 * (double)(CHUNK_SIZE * m_chunksPerPing) / fileSize;
    }

    return true;
}

bool StorageWriteOperation::processResponse(QObject *response)
{
    setProgress(m_chunksPerPing ? progress() + m_percentPerPing : 100.0);
    return qobject_cast<StatusPingResponseInterface*>(response) ||
           qobject_cast<EmptyResponseInterface*>(response);
}
