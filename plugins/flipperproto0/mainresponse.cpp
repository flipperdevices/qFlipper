#include "mainresponse.h"

#include <QHash>

#include "guiresponse.h"
#include "statusresponse.h"
#include "systemresponse.h"
#include "storageresponse.h"
#include "propertyresponse.h"

MainResponse::MainResponse(MessageWrapper &wrapper, QObject *parent):
    QObject(parent),
    m_wrapper(std::move(wrapper))
{}

MainResponse::~MainResponse()
{}

uint32_t MainResponse::id() const
{
    return m_wrapper.message().command_id;
}

bool MainResponse::hasNext() const
{
    return m_wrapper.message().has_next;
}

bool MainResponse::isError() const
{
    // Only severe errors are considered as such.
    // Other errors are handled inside the respective operations.
    const auto status = m_wrapper.message().command_status;
    switch (status) {
    case PB_CommandStatus_ERROR:
    case PB_CommandStatus_ERROR_DECODE:
    case PB_CommandStatus_ERROR_NOT_IMPLEMENTED:
    case PB_CommandStatus_ERROR_BUSY:
    case PB_CommandStatus_ERROR_CONTINUOUS_COMMAND_INTERRUPTED:
        return true;
    default:
        return false;
    }
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

    return statusStrings[m_wrapper.message().command_status];
}

QObject *MainResponse::create(MessageWrapper &wrapper, QObject *parent)
{
    if(!wrapper.isComplete()) {
        return nullptr;
    }

    const auto type = tagToResponseType(wrapper.message().which_content);

    switch(type) {
    case Empty: return new EmptyResponse(wrapper, parent);
    case StatusPing: return new StatusPingResponse(wrapper, parent);
    case SystemDeviceInfo: return new SystemDeviceInfoResponse(wrapper, parent);
    case SystemGetDateTime: return new SystemGetDateTimeResponse(wrapper, parent);
    case SystemUpdate: return new SystemUpdateResponse(wrapper, parent);
    case SystemProtobufVersion: return new SystemProtobufVersionResponse(wrapper, parent);
    case StorageInfo: return new StorageInfoResponse(wrapper, parent);
    case StorageStat: return new StorageStatResponse(wrapper, parent);
    case StorageList: return new StorageListResponse(wrapper, parent);
    case StorageRead: return new StorageReadResponse(wrapper, parent);
    case StorageMd5Sum: return new StorageMd5SumResponse(wrapper, parent);
    case GuiScreenFrame: return new GuiScreenFrameResponse(wrapper, parent);
    case PropertyGet: return new PropertyGetResponse(wrapper, parent);
    case Unknown:
    default: return nullptr;
    }
}

const PB_Main &MainResponse::message() const
{
    return m_wrapper.message();
}

MainResponseInterface::ResponseType MainResponse::tagToResponseType(pb_size_t tag)
{
    switch(tag) {
    case PB_Main_empty_tag: return Empty;

    case PB_Main_system_ping_response_tag: return StatusPing;
    case PB_Main_system_device_info_response_tag: return SystemDeviceInfo;
    case PB_Main_system_get_datetime_response_tag: return SystemGetDateTime;
    case PB_Main_system_update_response_tag: return SystemUpdate;
    case PB_Main_system_protobuf_version_response_tag: return SystemProtobufVersion;

    case PB_Main_storage_list_response_tag: return StorageList;
    case PB_Main_storage_read_response_tag: return StorageRead;
    case PB_Main_storage_md5sum_response_tag: return StorageMd5Sum;
    case PB_Main_storage_stat_response_tag: return StorageStat;
    case PB_Main_storage_info_response_tag: return StorageInfo;

    case PB_Main_gui_screen_frame_tag: return GuiScreenFrame;

    case PB_Main_property_get_response_tag: return PropertyGet;
    default: return Unknown;
    }
}

MainResponseInterface::ResponseType MainResponse::type() const
{
    return tagToResponseType(m_wrapper.message().which_content);
}

size_t MainResponse::encodedSize() const
{
    return m_wrapper.encodedSize();
}
