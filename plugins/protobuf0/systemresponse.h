#pragma once

#include "mainresponse.h"
#include "systemresponseinterface.h"

class SystemPingResponse : public MainResponse, public SystemPingResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemPingResponseInterface)

public:
    SystemPingResponse(MessageWrapper &&decoder, QObject *parent = nullptr);
    const QByteArray data() const override;
};

class SystemDeviceInfoResponse : public MainResponse, public SystemDeviceInfoResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemDeviceInfoResponseInterface)

public:
    SystemDeviceInfoResponse(MessageWrapper &&decoder, QObject *parent = nullptr);
    const QByteArray key() const override;
    const QByteArray value() const override;
};

class SystemDateTimeResponse : public MainResponse, public SystemDateTimeResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemDateTimeResponseInterface)

public:
    SystemDateTimeResponse(MessageWrapper &&decoder, QObject *parent = nullptr);
    const QDateTime dateTime() const override;
};
