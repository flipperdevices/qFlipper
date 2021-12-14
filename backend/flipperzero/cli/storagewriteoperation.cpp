#include "storagewriteoperation.h"

#include <QTimer>
#include <QIODevice>

#include "flipperzero/protobuf/storageprotobufmessage.h"

static constexpr qint64 CHUNK_SIZE = 512;

using namespace Flipper;
using namespace Zero;

StorageWriteOperation::StorageWriteOperation(QSerialPort *serialPort, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_path(path),
    m_file(file),
    m_byteCount(0)
{}

const QString StorageWriteOperation::description() const
{
    return QStringLiteral("Storage write @%1").arg(QString(m_path));
}

void StorageWriteOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    if(!response.receive()) {
        return;
    } else if(!response.isOk()) {
        finishWithError(QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
    } else if(!response.isValidType()) {
        finishWithError(QStringLiteral("Expected empty response, got something else"));
    } else {
        finish();
    }
}

void StorageWriteOperation::onTotalBytesWrittenChanged()
{
    if(totalBytesWritten() != m_byteCount) {
        return;
    }

    const auto bytesAvailable = m_file->bytesAvailable();

    if(bytesAvailable < 0) {
        finishWithError(QStringLiteral("Failed to read from input device: %1").arg(m_file->errorString()));

    } else if(bytesAvailable > 0) {
        QTimer::singleShot(0, this, [=]() {
            if(!writeChunk()) {
                finishWithError(QStringLiteral("Failed to write chunk"));
            }
        });
    }
}

bool StorageWriteOperation::begin()
{
    return writeChunk();
}

bool StorageWriteOperation::writeChunk()
{
    const auto hasNext = m_file->bytesAvailable() > CHUNK_SIZE;
    StorageWriteRequest request(serialPort(), m_path, m_file->read(CHUNK_SIZE), hasNext);

    const auto success = request.send();
    m_byteCount += request.bytesWritten();
    return success;
}
