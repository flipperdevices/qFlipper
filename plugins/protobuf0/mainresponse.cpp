#include "mainresponse.h"

#include <QHash>

#include "pb_decode.h"

MainResponse::MainResponse():
    m_message{69, PB_CommandStatus_ERROR_STORAGE_NOT_IMPLEMENTED, false, {}, PB_Main_empty_tag, {}}
{}

MainResponse::~MainResponse()
{
    // TODO: release the resources if necessary
//    pb_release(&PB_Main_msg, &m_message);
}

uint32_t MainResponse::commandID() const
{
    return m_message.command_id;
}

bool MainResponse::hasNext() const
{
    return m_message.has_next;
}

bool MainResponse::isError() const
{
    return m_message.command_status != PB_CommandStatus_OK;
}

const QString MainResponse::errorString() const
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

    return statusStrings[m_message.command_status];
}

const PB_Main &MainResponse::message() const
{
    return m_message;
}

MainResponseInterface::ResponseType MainResponse::type() const
{
    switch(m_message.which_content) {
    case PB_Main_empty_tag: return Empty;

    case PB_Main_system_ping_response_tag: return SystemPing;
    case PB_Main_system_device_info_response_tag: return SystemDeviceInfo;
    case PB_Main_system_get_datetime_response_tag: return SystemGetDateTime;

    case PB_Main_storage_list_response_tag: return StorageList;
    case PB_Main_storage_read_response_tag: return StorageRead;
    case PB_Main_storage_md5sum_response_tag: return StorageMd5;
    case PB_Main_storage_stat_response_tag: return StorageStat;
    case PB_Main_storage_info_response_tag: return StorageInfo;

    case PB_Main_gui_screen_frame_tag: return GuiScreenFrame;
    default: return Unknown;
    }
}
