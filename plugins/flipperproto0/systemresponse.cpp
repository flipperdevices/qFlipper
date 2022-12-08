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

SystemUpdateResponse::SystemUpdateResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

bool SystemUpdateResponse::isResultOk() const
{
    return message().content.system_update_response.code == PB_System_UpdateResponse_UpdateResultCode_OK;
}

const QString SystemUpdateResponse::resultString() const
{
    switch(message().content.system_update_response.code) {
    case PB_System_UpdateResponse_UpdateResultCode_OK:
        return QStringLiteral("System update is go");
    case PB_System_UpdateResponse_UpdateResultCode_ManifestPathInvalid:
        return QStringLiteral("Manifest path invalid");
    case PB_System_UpdateResponse_UpdateResultCode_ManifestFolderNotFound:
        return QStringLiteral("Manifest folder not found");
    case PB_System_UpdateResponse_UpdateResultCode_ManifestInvalid:
        return QStringLiteral("Manifest is invalid");
    case PB_System_UpdateResponse_UpdateResultCode_StageMissing:
        return QStringLiteral("Stage missing");
    case PB_System_UpdateResponse_UpdateResultCode_StageIntegrityError:
        return QStringLiteral("Stage integrity error");
    case PB_System_UpdateResponse_UpdateResultCode_ManifestPointerError:
        return QStringLiteral("Manifest pointer error");
    case PB_System_UpdateResponse_UpdateResultCode_TargetMismatch:
        return QStringLiteral("Target mismatch");
    case PB_System_UpdateResponse_UpdateResultCode_OutdatedManifestVersion:
        return QStringLiteral("Outdated manifest version");
    case PB_System_UpdateResponse_UpdateResultCode_IntFull:
        return QStringLiteral("Internal storage is full");
    case PB_System_UpdateResponse_UpdateResultCode_UnspecifiedError:
    default:
        return QStringLiteral("Unknown error");
    }
}

SystemProtobufVersionResponse::SystemProtobufVersionResponse(MessageWrapper &wrapper, QObject *parent):
    MainResponse(wrapper, parent)
{}

uint32_t SystemProtobufVersionResponse::versionMajor() const
{
    return message().content.system_protobuf_version_response.major;
}

uint32_t SystemProtobufVersionResponse::versionMinor() const
{
    return message().content.system_protobuf_version_response.minor;
}
