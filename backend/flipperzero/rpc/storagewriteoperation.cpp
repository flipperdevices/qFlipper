#include "storagewriteoperation.h"

#include <QTimer>
#include <QIODevice>

//static constexpr qint64 CHUNK_SIZE = 512;

using namespace Flipper;
using namespace Zero;

StorageWriteOperation::StorageWriteOperation(QSerialPort *serialPort, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path),
    m_file(file),
    m_byteCount(0)
{
    // Write operations can be lenghty
    setTimeout(30000);
}

const QString StorageWriteOperation::description() const
{
    return QStringLiteral("Storage write @%1").arg(QString(m_path));
}

void StorageWriteOperation::onSerialPortReadyRead()
{
}

void StorageWriteOperation::onTotalBytesWrittenChanged()
{
    if(totalBytesWritten() != m_byteCount) {
        return;
    }

    const auto bytesAvailable = m_file->bytesAvailable();

    if(bytesAvailable < 0) {
        finishWithError(BackendError::DiskError, QStringLiteral("Failed to read from input device: %1").arg(m_file->errorString()));

    } else if(bytesAvailable > 0) {
        // Must write the chunk asynchronously in order to receive this signal
        QTimer::singleShot(0, this, [=]() {
            if(!writeChunk()) {
                finishWithError(BackendError::SerialError, QStringLiteral("Failed to write chunk"));
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
    return false;
}
