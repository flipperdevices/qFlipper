#pragma once

#include "abstractserialoperation.h"

class AbstractProtobufOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:
    AbstractProtobufOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    virtual ~AbstractProtobufOperation();
};

