#pragma once

#include "mainresponse.h"
#include "propertyresponseinterface.h"

class PropertyGetResponse : public MainResponse, public PropertyGetResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(PropertyGetResponseInterface)

public:
    PropertyGetResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    const QByteArray key() const override;
    const QByteArray value() const override;
};

