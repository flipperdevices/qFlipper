#include "systemrebootoperation.h"

#include <QTimer>
#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

SystemRebootOperation::SystemRebootOperation(QSerialPort *serialPort, RebootType rebootType, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
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
    return false;
}
