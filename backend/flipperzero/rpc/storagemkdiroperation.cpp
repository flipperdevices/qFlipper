#include "storagemkdiroperation.h"
#include "flipperzero/protobuf/storageprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StorageMkdirOperation::StorageMkdirOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path)
{}

const QString StorageMkdirOperation::description() const
{
    return QStringLiteral("Storage mkdir @%1").arg(QString(m_path));
}

void StorageMkdirOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    if(!response.receive()) {
        return;
    } else if(!response.isOk() && (response.commandStatus() != PB_CommandStatus_ERROR_STORAGE_EXIST)) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
    } else if(!response.isValidType()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Expected empty response, got something else"));
    } else {
        finish();
    }
}

bool StorageMkdirOperation::begin()
{
    StorageMkdirRequest request(serialPort(), m_path);
    return request.send();
}
