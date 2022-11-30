#pragma once

#include "mainrequest.h"

#include <QDateTime>

class SystemRebootRequest : public MainRequest
{
public:
    SystemRebootRequest(uint32_t id, PB_System_RebootRequest_RebootMode mode);
};

class SystemFactoryResetRequest : public MainRequest
{
public:
    SystemFactoryResetRequest(uint32_t id);
};

class SystemDeviceInfoRequest : public MainRequest
{
public:
    SystemDeviceInfoRequest(uint32_t id);
};

class SystemGetDateTimeRequest : public MainRequest
{
public:
    SystemGetDateTimeRequest(uint32_t id);
};

class SystemSetDateTimeRequest : public MainRequest
{
public:
    SystemSetDateTimeRequest(uint32_t id, const QDateTime &dateTime);
};

class SystemUpdateRequest : public MainRequest
{
public:
    SystemUpdateRequest(uint32_t id, const QByteArray &manifestPath);
private:
    QByteArray m_manifestPath;
};

class SystemProtobufVersionRequest : public MainRequest
{
public:
    SystemProtobufVersionRequest(uint32_t id);
};
