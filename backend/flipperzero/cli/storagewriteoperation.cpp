#include "storagewriteoperation.h"

#include <QDebug>
#include <QIODevice>
#include <QSerialPort>

#include "flipperzero/protobuf/storageprotobufmessage.h"

static constexpr qint64 CHUNK_SIZE = 512;

using namespace Flipper;
using namespace Zero;

StorageWriteOperation::StorageWriteOperation(QSerialPort *serialPort, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_path(path),
    m_file(file)
{}

const QString StorageWriteOperation::description() const
{
    return QStringLiteral("Storage Write @%1").arg(QString(m_path));
}

void StorageWriteOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    while(response.receive()) {
        if(!response.isOk()) {
            finishWithError(QStringLiteral("Failed to write data: device replied with an error"));
        } else if(!response.isValidType()) {
            finishWithError(QStringLiteral("Expected to get an empty response, got something else"));
        } else if(!response.hasNext()) {
            finish();
        } else {
            continue;
        }

        break;
    }
}

void StorageWriteOperation::onTotalBytesWrittenChanged()
{
#warning This is obviously wrong. What was I thinking?
    if((totalBytesWritten() == m_file->pos()) && (m_file->bytesAvailable() > 0)) {
        if(!writeChunk()) {
            finishWithError(QStringLiteral("Failed to write chunk"));
        }
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
    return request.send();
}
