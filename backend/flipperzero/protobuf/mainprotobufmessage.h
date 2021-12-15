#pragma once

#include <QHash>

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
    const QString commandStatusString() const;
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
const QString AbstractMainProtobufResponse<Tag>::commandStatusString() const
{
    static const QHash<PB_CommandStatus, QString> statusStrings = {
       {PB_CommandStatus_OK, QStringLiteral("No error")},
        // Common errors
       {PB_CommandStatus_ERROR, QStringLiteral("Unknown")},
       {PB_CommandStatus_ERROR_DECODE, QStringLiteral("Decode failure")},
       {PB_CommandStatus_ERROR_NOT_IMPLEMENTED, QStringLiteral("Commant not implemented")},
       {PB_CommandStatus_ERROR_BUSY, QStringLiteral("Device is busy")},
       {PB_CommandStatus_ERROR_CONTINUOUS_COMMAND_INTERRUPTED, QStringLiteral("Continuous command interrupted")},
       {PB_CommandStatus_ERROR_INVALID_PARAMETERS, QStringLiteral("Invalid parameters")},
        // Storage errors
       {PB_CommandStatus_ERROR_STORAGE_NOT_READY, QStringLiteral("Storage not ready")},
       {PB_CommandStatus_ERROR_STORAGE_EXIST, QStringLiteral("File/directory already exists")},
       {PB_CommandStatus_ERROR_STORAGE_NOT_EXIST, QStringLiteral("File/directory does not exist")},
       {PB_CommandStatus_ERROR_STORAGE_INVALID_PARAMETER, QStringLiteral("Invalid storage API parameter")},
       {PB_CommandStatus_ERROR_STORAGE_DENIED, QStringLiteral("Access denied")},
       {PB_CommandStatus_ERROR_STORAGE_INVALID_NAME, QStringLiteral("Invalid name/path")},
       {PB_CommandStatus_ERROR_STORAGE_INTERNAL, QStringLiteral("Internal error")},
       {PB_CommandStatus_ERROR_STORAGE_NOT_IMPLEMENTED, QStringLiteral("Storage command not implemented")},
       {PB_CommandStatus_ERROR_STORAGE_ALREADY_OPEN, QStringLiteral("File/directory is already open")},
       {PB_CommandStatus_ERROR_STORAGE_DIR_NOT_EMPTY, QStringLiteral("Directory is not empty")},
        // Application errors
       {PB_CommandStatus_ERROR_APP_CANT_START, QStringLiteral("Cannot start the application")},
       {PB_CommandStatus_ERROR_APP_SYSTEM_LOCKED, QStringLiteral("Another application is already running")},
        // Virtual display errors
       {PB_CommandStatus_ERROR_VIRTUAL_DISPLAY_ALREADY_STARTED, QStringLiteral("Virtual display session has already been started")},
       {PB_CommandStatus_ERROR_VIRTUAL_DISPLAY_NOT_STARTED, QStringLiteral("No virtual display session running")}
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

