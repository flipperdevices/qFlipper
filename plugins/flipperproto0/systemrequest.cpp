#include "systemrequest.h"

#include "pb_encode.h"

SystemRebootRequest::SystemRebootRequest(uint32_t id, PB_System_RebootRequest_RebootMode mode):
    MainRequest(id, PB_Main_system_reboot_request_tag)
{
    auto &content = m_message.content.system_reboot_request;
    content.mode = mode;
}

SystemDeviceInfoRequest::SystemDeviceInfoRequest(uint32_t id):
    MainRequest(id, PB_Main_system_device_info_request_tag)
{}

SystemGetDateTimeRequest::SystemGetDateTimeRequest(uint32_t id):
    MainRequest(id, PB_Main_system_get_datetime_request_tag)
{}

SystemSetDateTimeRequest::SystemSetDateTimeRequest(uint32_t id, const QDateTime &dateTime):
    MainRequest(id, PB_Main_system_set_datetime_request_tag)
{
    auto &content = m_message.content.system_set_datetime_request;
    auto &ts = content.datetime;

    ts.day = dateTime.date().day();
    ts.month = dateTime.date().month();
    ts.weekday = dateTime.date().dayOfWeek();
    ts.year = dateTime.date().year();

    ts.hour = dateTime.time().hour();
    ts.minute = dateTime.time().minute();
    ts.second = dateTime.time().second();

    content.has_datetime = true;
}

SystemFactoryResetRequest::SystemFactoryResetRequest(uint32_t id):
    MainRequest(id, PB_Main_system_factory_reset_request_tag)
{}

SystemUpdateRequest::SystemUpdateRequest(uint32_t id, const QByteArray &manifestPath):
    MainRequest(id, PB_Main_system_update_request_tag),
    m_manifestPath(manifestPath)
{
    auto &content = m_message.content.system_update_request;
    content.update_manifest = m_manifestPath.data();
}

SystemProtobufVersionRequest::SystemProtobufVersionRequest(uint32_t id):
    MainRequest(id, PB_Main_system_protobuf_version_request_tag)
{}
