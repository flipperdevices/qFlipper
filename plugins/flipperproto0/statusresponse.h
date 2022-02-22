#pragma once

#include "mainresponse.h"
#include "statusresponseinterface.h"

class StatusPingResponse : public MainResponse, public StatusPingResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(StatusPingResponseInterface)

public:
    StatusPingResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    const QByteArray data() const override;
};
