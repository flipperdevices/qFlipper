#include "systemrebootoperation.h"

#include <QTimer>
#include <QSerialPort>

#include "flipperzero/protobuf/systemprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

SystemRebootOperation::SystemRebootOperation(QSerialPort *serialPort, RebootType rebootType, QObject *parent):
    AbstractProtobufOperation(serialPort, parent),
    m_rebootType(rebootType),
    m_byteCount(0)
{}

const QString SystemRebootOperation::description() const
{
    return QStringLiteral("System reboot @%1").arg(QString(serialPort()->portName()));
}

void SystemRebootOperation::onTotalBytesWrittenChanged()
{
    if(m_byteCount == totalBytesWritten()) {
        finish();
    }
}

bool SystemRebootOperation::begin()
{
    const auto rebootType = m_rebootType == RebootType::OS ? PB_System_RebootRequest_RebootMode_OS :
                                                             PB_System_RebootRequest_RebootMode_DFU;
    SystemRebootRequest request(serialPort(), rebootType);
    const auto success = request.send();
    m_byteCount = request.bytesWritten();
    return success;
}
