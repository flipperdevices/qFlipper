#include "abstractprotobufoperation.h"

AbstractProtobufOperation::AbstractProtobufOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}
