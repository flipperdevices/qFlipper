#include "systemdeviceinfooperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

SystemDeviceInfoOperation::SystemDeviceInfoOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString SystemDeviceInfoOperation::description() const
{
    return QStringLiteral("Device Info @%1").arg(QString(serialPort()->portName()));
}

const QByteArray SystemDeviceInfoOperation::result(const QByteArray &key) const
{
    return m_data.value(key);
}

void SystemDeviceInfoOperation::onSerialPortReadyRead()
{
    SystemDeviceInfoResponse response(serialPort());

    while(response.receive()) {

        if(!response.isOk()) {
            finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with an error response"));
            return;
        } else if(!response.isValidType()) {
            finishWithError(BackendError::ProtocolError, QStringLiteral("Expected empty reply, got something else"));
            return;
        }

        m_data.insert(response.key(), response.value());

        if(!response.hasNext()) {
            finish();
            return;
        }
    }
}

bool SystemDeviceInfoOperation::begin()
{
    SystemDeviceInfoRequest request(serialPort());
    return request.send();
}
