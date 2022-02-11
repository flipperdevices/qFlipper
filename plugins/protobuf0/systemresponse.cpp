#include "systemresponse.h"

SystemPingResponse::SystemPingResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QByteArray SystemPingResponse::data() const
{
    const auto *d = message().content.system_ping_response.data;
    return QByteArray((const char*)d->bytes, d->size);
}

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

SystemDateTimeResponse::SystemDateTimeResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

const QDateTime SystemDateTimeResponse::dateTime() const
{
    if(!message().content.system_get_datetime_response.has_datetime) {
        return QDateTime();
    }

    const auto &ts = message().content.system_get_datetime_response.datetime;

    const QDate date(ts.year, ts.month, ts.day);
    const QTime time(ts.hour, ts.minute, ts.second);

    return QDateTime(date, time);
}
