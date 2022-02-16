#include "systemgetdatetimeoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

SystemGetDateTimeOperation::SystemGetDateTimeOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString SystemGetDateTimeOperation::description() const
{
    return QStringLiteral("Get DateTime @%1").arg(QString(serialPort()->portName()));
}

const QDateTime &SystemGetDateTimeOperation::dateTime() const
{
    return m_dateTime;
}

void SystemGetDateTimeOperation::onSerialPortReadyRead()
{
    SystemGetDateTimeResponse response(serialPort());

    if(!response.receive()) {
        return;
    } else if(!response.isOk()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
    } else if(!response.isValidType()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Expected SystemGetDateTime response, got something else"));
    } else {
        m_dateTime = response.dateTime();
        finish();
    }
}

bool SystemGetDateTimeOperation::begin()
{
    SystemGetDateTimeRequest request(serialPort());
    return request.send();
}
