#pragma once

#include "mainrequest.h"

class PropertyGetRequest : public MainRequest
{
public:
    PropertyGetRequest(uint32_t id, const QByteArray &key);
private:
    QByteArray m_key;
};

