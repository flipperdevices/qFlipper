#pragma once

#include "abstractprotobufmessage.h"
#include "messages/flipper.pb.h"

namespace Flipper {
namespace Zero {

template<const pb_size_t Tag>
class AbstractMainProtobufRequest : public AbstractProtobufRequest<&PB_Main_msg, PB_Main>
{
public:
    AbstractMainProtobufRequest(QSerialPort *serialPort, bool hasNext = false);
};

template<const pb_size_t Tag>
class AbstractMainProtobufResponse : public AbstractProtobufResponse<&PB_Main_msg, PB_Main>
{
public:
    AbstractMainProtobufResponse(QSerialPort *serialPort):
        AbstractProtobufResponse<&PB_Main_msg, PB_Main>(serialPort)
    {}

    virtual ~AbstractMainProtobufResponse() {}

    bool hasNext() const;
    PB_CommandStatus commandStatus() const;
    const QString &commandStatusString() const;
    quint32 whichContent() const;

    bool isOk() const;
    bool isValidType() const;

    static pb_size_t tag();
};

template<const pb_size_t Tag>
AbstractMainProtobufRequest<Tag>::AbstractMainProtobufRequest(QSerialPort *serialPort, bool hasNext):
    AbstractProtobufRequest<&PB_Main_msg, PB_Main>(serialPort)
{
    pbMessage()->which_content = Tag;
    pbMessage()->has_next = hasNext;
}


template<const pb_size_t Tag>
bool AbstractMainProtobufResponse<Tag>::hasNext() const
{
    return pbMessage()->has_next;
}

template<const pb_size_t Tag>
PB_CommandStatus AbstractMainProtobufResponse<Tag>::commandStatus() const
{
    return pbMessage()->command_status;
}

template<const pb_size_t Tag>
const QString &AbstractMainProtobufResponse<Tag>::commandStatusString() const
{
#warning Fix the string order
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

template<const pb_size_t Tag>
quint32 AbstractMainProtobufResponse<Tag>::whichContent() const
{
    return pbMessage()->which_content;
}

template<const pb_size_t Tag>
bool AbstractMainProtobufResponse<Tag>::isOk() const
{
    return commandStatus() == PB_CommandStatus_OK;
}

template<const pb_size_t Tag>
bool AbstractMainProtobufResponse<Tag>::isValidType() const
{
    return pbMessage()->which_content == Tag;
}

template<const pb_size_t Tag>
pb_size_t AbstractMainProtobufResponse<Tag>::tag()
{
    return Tag;
}

class MainStopSessionRequest : public AbstractMainProtobufRequest<PB_Main_stop_session_tag>
{
public:
    MainStopSessionRequest(QSerialPort *serialPort);
};

class MainEmptyResponse : public AbstractMainProtobufResponse<PB_Main_empty_tag>
{
public:
    MainEmptyResponse(QSerialPort *serialPort);
};

}
}

