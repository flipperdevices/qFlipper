#pragma once

#include "abstractserialoperation.h"

#include <pb.h>

class AbstractProtobufOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:
    AbstractProtobufOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    virtual ~AbstractProtobufOperation() {};

protected:
    bool sendMessage(const pb_msgdesc_t *fields, void *msg);
    bool receiveMessage(const pb_msgdesc_t *fields, void *msg);

private:
    static bool inputCallback(pb_istream_t *stream, pb_byte_t *buf, size_t count);
    static bool outputCallback(pb_ostream_t *stream, const pb_byte_t *buf, size_t count);
};

