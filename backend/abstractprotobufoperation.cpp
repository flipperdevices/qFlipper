#include "abstractprotobufoperation.h"

#include <QDebug>
#include <QSerialPort>
#include <QLoggingCategory>

AbstractProtobufOperation::AbstractProtobufOperation(QSerialPort *serialPort, QObject *parent):
    AbstractSerialOperation(serialPort, parent)
{}

AbstractProtobufOperation::~AbstractProtobufOperation()
{}
