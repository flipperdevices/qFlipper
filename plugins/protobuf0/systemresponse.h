#pragma once

#include "mainresponse.h"
#include "systemresponseinterface.h"

class SystemPingResponse : public MainResponse, public SystemPingResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemPingResponseInterface)

public:
    const QByteArray data() const override;
};

class SystemDeviceInfoResponse : public MainResponse, public SystemDeviceInfoResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemDeviceInfoResponseInterface)

public:
    const QByteArray key() const override;
    const QByteArray value() const override;
};

class SystemDateTimeResponse : public MainResponse, public SystemDateTimeResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemDateTimeResponseInterface)

public:
    const QDateTime dateTime() const override;
};
