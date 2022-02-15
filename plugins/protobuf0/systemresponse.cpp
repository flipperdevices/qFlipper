#include "systemresponse.h"

SystemDeviceInfoResponse::SystemDeviceInfoResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QByteArray SystemDeviceInfoResponse::key() const
{
    return message().content.system_device_info_response.key;
}

const QByteArray SystemDeviceInfoResponse::value() const
{
    return message().content.system_device_info_response.value;
}

SystemGetDateTimeResponse::SystemGetDateTimeResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QDateTime SystemGetDateTimeResponse::dateTime() const
{
    if(!message().content.system_get_datetime_response.has_datetime) {
        return QDateTime();
    }

    const auto &ts = message().content.system_get_datetime_response.datetime;

    const QDate date(ts.year, ts.month, ts.day);
    const QTime time(ts.hour, ts.minute, ts.second);

    return QDateTime(date, time);
}
