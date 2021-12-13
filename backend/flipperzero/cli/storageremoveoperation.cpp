#include "storageremoveoperation.h"

#include "flipperzero/protobuf/storageprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StorageRemoveOperation::StorageRemoveOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_path(path)
{}

const QString StorageRemoveOperation::description() const
{
    return QStringLiteral("Storage Remove @%1").arg(QString(m_path));
}

void StorageRemoveOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    if(!response.receive()) {
        return;
    } else if(!response.isOk()) {
        finishWithError(QStringLiteral("Cannot remove file/directory: %1").arg(response.commandStatusString()));
    } else if(!response.isValidType()) {
        finishWithError(QStringLiteral("Expected empty response, got something else"));
    } else {
        finish();
    }
}

bool StorageRemoveOperation::begin()
{
    StorageRemoveRequest request(serialPort(), m_path);
    return request.send();
}
