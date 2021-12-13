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

template<const pb_size_t Tag>
const QString &AbstractMainProtobufResponse<Tag>::commandStatusString() const
{
    static const QString statusStrings[] = {
        QStringLiteral("No error"),
        // Common errors
        QStringLiteral("Unknown"),
        QStringLiteral("Decode failure"),
        QStringLiteral("Commant not implemented"),
        QStringLiteral("Device is busy"),
        QStringLiteral("Continuous command interrupted"),
        QStringLiteral("Invalid parameters"),
        // Storage errors
        QStringLiteral("Storage not ready"),
        QStringLiteral("File/directory already exists"),
        QStringLiteral("File/directory does not exist"),
        QStringLiteral("Invalid storage API parameter"),
        QStringLiteral("Access denied"),
        QStringLiteral("Invalid name/path"),
        QStringLiteral("Internal error"),
        QStringLiteral("Storage command not implemented"),
        QStringLiteral("File/directory is already open"),
        QStringLiteral("Directory is not empty"),
        // Application errors
        QStringLiteral("Cannot start the application"),
        QStringLiteral("Another application is already running"),
        // Virtual display errors
        QStringLiteral("Virtual display session has already been started"),
        QStringLiteral("No virtual display session running")
    };

    return statusStrings[commandStatus()];
}
