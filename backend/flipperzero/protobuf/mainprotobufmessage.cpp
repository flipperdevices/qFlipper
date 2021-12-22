#include "mainprotobufmessage.h"

using namespace Flipper;
using namespace Zero;

MainEmptyResponse::MainEmptyResponse(QSerialPort *serialPort):
    AbstractMainProtobufResponse(serialPort)
{}

MainStopSessionRequest::MainStopSessionRequest(QSerialPort *serialPort):
    AbstractMainProtobufRequest(serialPort)
{
    pbMessage()->content.stop_session = PB_StopSession_init_default;
}
