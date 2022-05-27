#pragma once

#include "mainresponse.h"
#include "systemresponseinterface.h"

class SystemDeviceInfoResponse : public MainResponse, public SystemDeviceInfoResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemDeviceInfoResponseInterface)

public:
    SystemDeviceInfoResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    const QByteArray key() const override;
    const QByteArray value() const override;
};

class SystemGetDateTimeResponse : public MainResponse, public SystemGetDateTimeResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemGetDateTimeResponseInterface)

public:
    SystemGetDateTimeResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    const QDateTime dateTime() const override;
};

class SystemUpdateResponse : public MainResponse, public SystemUpdateResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(SystemUpdateResponseInterface)

public:
    SystemUpdateResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    bool isResultOk() const;
    const QString resultString() const;
};
