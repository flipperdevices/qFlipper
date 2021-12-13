#include "storagemkdiroperation.h"
#include "flipperzero/protobuf/storageprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StorageMkdirOperation::StorageMkdirOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_path(path)
{}

const QString StorageMkdirOperation::description() const
{
    return QStringLiteral("Storage Mkdir @%1").arg(QString(m_path));
}

void StorageMkdirOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    if(!response.receive()) {
        return;
    } else if(!response.isOk()) {
        finishWithError(QStringLiteral("Device replied with an error response: %1").arg(response.commandStatusString()));
    } else if(!response.isValidType()) {
        finishWithError(QStringLiteral("Expected Empty response, got something else"));
    } else {
        finish();
    }
}

bool StorageMkdirOperation::begin()
{
    StorageMkdirRequest request(serialPort(), m_path);
    return request.send();
}
