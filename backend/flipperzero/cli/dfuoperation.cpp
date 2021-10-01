#include "dfuoperation.h"

#include <QTimer>
#include <QSerialPort>

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

using namespace Flipper;
using namespace Zero;

DFUOperation::DFUOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

const QString DFUOperation::description() const
{
    return QStringLiteral("Boot to recovery @%1").arg(QString(serialPort()->portName()));
}

void DFUOperation::onSerialPortReadyRead()
{
    // This operation does not need serial output, discarding it
    serialPort()->clear();
}

bool DFUOperation::begin()
{
    const auto success = (serialPort()->write("\rdfu\r\n") > 0);

    if(success) {
        CALL_LATER(this, &AbstractOperation::finish);
    }

    return success;
}
