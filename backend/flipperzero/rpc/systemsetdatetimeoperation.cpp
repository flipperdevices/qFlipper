#include "systemsetdatetimeoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

SystemSetDateTimeOperation::SystemSetDateTimeOperation(QSerialPort *serialPort, const QDateTime &dateTime, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_dateTime(dateTime)
{}

const QString SystemSetDateTimeOperation::description() const
{
    return QStringLiteral("Set DateTime @%1").arg(QString(serialPort()->portName()));
}

void SystemSetDateTimeOperation::onSerialPortReadyRead()
{
    MainEmptyResponse response(serialPort());

    if(!response.receive()) {
        return;
    } else if(!response.isOk()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with error: %1").arg(response.commandStatusString()));
    } else if(!response.isValidType()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Expected empty response, got something else"));
    } else {
        finish();
    }
}

bool SystemSetDateTimeOperation::begin()
{
    SystemSetDateTimeRequest request(serialPort(), m_dateTime);
    return request.send();
}
