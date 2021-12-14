#include "systemfactoryresetoperation.h"

#include <QSerialPort>

#include "flipperzero/protobuf/systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

SystemFactoryResetOperation::SystemFactoryResetOperation(QSerialPort *serialPort, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_byteCount(0)
{}

const QString SystemFactoryResetOperation::description() const
{
    return QStringLiteral("Factory reset @%1").arg(QString(serialPort()->portName()));
}

void SystemFactoryResetOperation::onTotalBytesWrittenChanged()
{
    if(m_byteCount == totalBytesWritten()) {
        finish();
    }
}

bool SystemFactoryResetOperation::begin()
{
    SystemFactoryResetRequest request(serialPort());
    const auto success = request.send();
    m_byteCount = request.bytesWritten();
    return success;
}
