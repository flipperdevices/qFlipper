#include "rebootoperation.h"

#include <QTimer>
#include <QSerialPort>

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

RebootOperation::RebootOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString RebootOperation::description() const
{
    return QStringLiteral("Reboot @%1").arg(QString(serialPort()->portName()));
}

void RebootOperation::onSerialPortReadyRead()
{
    // This operation does not need serial output, discarding it
    serialPort()->clear();
}

bool RebootOperation::begin()
{
    const auto success = (serialPort()->write("\rreboot\r\n") > 0) && serialPort()->flush();

    if(success) {
        CALL_LATER(this, &AbstractOperation::finish);
    }

    return success;
}
