#pragma once

#include "mainrequest.h"

class StatusPingRequest : public MainRequest
{
public:
    StatusPingRequest(uint32_t id, const QByteArray &data = QByteArray());
    ~StatusPingRequest();
};
