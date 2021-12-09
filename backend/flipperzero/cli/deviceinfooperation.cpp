#include "deviceinfooperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

DeviceInfoOperation::DeviceInfoOperation(QSerialPort *serialPort, QObject *parent):
    AbstractProtobufOperation(serialPort, parent)
{}

const QString DeviceInfoOperation::description() const
{
    return QStringLiteral("Device Info @%1").arg(QString(serialPort()->portName()));
}

const QByteArray DeviceInfoOperation::result(const QByteArray &key) const
{
    return m_data.value(key);
}

void DeviceInfoOperation::onSerialPortReadyRead()
{
    SystemDeviceInfoResponse response(serialPort());

    while(response.receive()) {

        if(!response.isOk()) {
            finishWithError(QStringLiteral("Device replied with an error response"));
            return;
        } else if(!response.isValidType()) {
            finishWithError(QStringLiteral("Expected empty reply, got something else"));
            return;
        }

        m_data.insert(response.key(), response.value());

        if(!response.hasNext()) {
            finish();
            return;
        }
    }
}

bool DeviceInfoOperation::begin()
{
    SystemDeviceInfoRequest request(serialPort());
    return request.send();
}
