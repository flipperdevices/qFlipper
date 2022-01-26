#include "storagereadoperation.h"
#include "flipperzero/protobuf/storageprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StorageReadOperation::StorageReadOperation(QSerialPort *serialPort, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_path(path),
    m_file(file)
{}

const QString StorageReadOperation::description() const
{
    return QStringLiteral("Storage read @%1").arg(QString(m_path));
}

void StorageReadOperation::onSerialPortReadyRead()
{
    StorageReadResponse response(serialPort());

    while(response.receive()) {

        if(!response.isOk()) {
            finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
        } else if(!response.isValidType()) {
            finishWithError(BackendError::ProtocolError, QStringLiteral("Expected StorageRead response, got something else"));
        } else {
            const auto &data = response.data();
            const auto bytesWritten = m_file->write(response.data());

            if(bytesWritten != data.size()) {
                finishWithError(BackendError::DiskError, QStringLiteral("Error writing to output device: %1").arg(m_file->errorString()));
            } else if(!response.hasNext()) {
                rewindAndFinish();
            } else {
                continue;
            }
        }

        break;
    }
}

bool StorageReadOperation::begin()
{
    StorageReadRequest request(serialPort(), m_path);
    return request.send();
}

void StorageReadOperation::rewindAndFinish()
{
    if(!m_file->seek(0)) {
        finishWithError(BackendError::DiskError, QStringLiteral("Failed to rewind output device: %1").arg(m_file->errorString()));
    } else {
        finish();
    }
}
