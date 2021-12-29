#include "storageinfooperation.h"

#include "flipperzero/protobuf/storageprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

StorageInfoOperation::StorageInfoOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_path(path),
    m_isPresent(false),
    m_sizeFree(0),
    m_sizeTotal(0)
{}

const QString StorageInfoOperation::description() const
{
    return QStringLiteral("Storage info @%1").arg(QString(m_path));
}

bool StorageInfoOperation::isPresent() const
{
    return m_isPresent;
}

quint64 StorageInfoOperation::sizeFree() const
{
    return m_sizeFree;
}

quint64 StorageInfoOperation::sizeTotal() const
{
    return m_sizeTotal;
}

void StorageInfoOperation::onSerialPortReadyRead()
{
    StorageInfoResponse response(serialPort());

    if(!response.receive()) {
        return;

    } else if(!response.isOk()) {
        const auto status = response.commandStatus();
        // TODO: more flexible error handling
        if(status == PB_CommandStatus_ERROR_STORAGE_INTERNAL) {
            finish();
        } else{
            finishWithError(QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
        }

    } else if(!response.isValidType()) {
        finishWithError(QStringLiteral("Expected StorageInfo response, got something else"));

    } else {
        m_isPresent = true;
        m_sizeFree = response.sizeFree();
        m_sizeTotal = response.sizeTotal();

        finish();
    }
}

bool StorageInfoOperation::begin()
{
    StorageInfoRequest request(serialPort(), m_path);
    return request.send();
}
