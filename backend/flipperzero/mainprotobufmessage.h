#pragma once

#include <abstractprotobufmessage.h>

#include "protobuf/flipper.pb.h"

namespace Flipper {
namespace Zero {

class MainProtobufMessage : public AbstractProtobufMessage<PB_Main>
{
public:
    MainProtobufMessage(QSerialPort *serialPort);

//    static ProtobufMessage startScreenStream();

    bool hasNext() const;
    uint32_t commandId() const;
    pb_size_t whichContent() const;
    PB_CommandStatus commandStatus() const;

    const QByteArray screenFrame() const;
};

}
}

