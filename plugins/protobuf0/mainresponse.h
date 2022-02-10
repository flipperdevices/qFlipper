#pragma once

#include <QObject>

#include "mainresponseinterface.h"

#include "messages/flipper.pb.h"

class MainResponse : public QObject, public MainResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(MainResponseInterface)

public:
    MainResponse();
    virtual ~MainResponse();

    uint32_t commandID() const override;
    ResponseType type() const override;

    bool hasNext() const override;
    bool isError() const override;

    const QString errorString() const override;

protected:
    const PB_Main &message() const;

private:
    PB_Main m_message;
};
