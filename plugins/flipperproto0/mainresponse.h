#pragma once

#include <QObject>

#include "messagewrapper.h"
#include "mainresponseinterface.h"

class MainResponse : public QObject, public MainResponseInterface
{
    Q_OBJECT
    Q_INTERFACES(MainResponseInterface)

public:
    MainResponse(MessageWrapper &wrapper, QObject *parent = nullptr);
    virtual ~MainResponse();

    uint32_t id() const override;
    ResponseType type() const override;
    size_t encodedSize() const override;

    bool hasNext() const override;
    bool isError() const override;

    const QString errorString() const override;

    static QObject *create(MessageWrapper &wrapper, QObject *parent = nullptr);

protected:
    const PB_Main &message() const;

private:
    static ResponseType tagToResponseType(pb_size_t tag);
    MessageWrapper m_wrapper;
};

using EmptyResponse = MainResponse;
