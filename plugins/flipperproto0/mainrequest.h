#pragma once

#include <QByteArray>

#include "messages/flipper.pb.h"

class MainRequest
{
public:
    MainRequest(uint32_t id, pb_size_t tag, bool hasNext = false);
    virtual ~MainRequest() {}

    const QByteArray encode() const;

protected:
    PB_Main m_message;
};

