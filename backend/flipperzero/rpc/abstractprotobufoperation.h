#pragma once

#include "abstractoperation.h"

class ProtobufPluginInterface;

namespace Flipper {
namespace Zero {

class AbstractProtobufOperation : public AbstractOperation
{
    Q_OBJECT

    enum ProtobufOperationState {
        Started = AbstractOperation::User
    };

public:
    AbstractProtobufOperation(uint32_t id, QObject *parent = nullptr);
    virtual ~AbstractProtobufOperation();

    uint32_t id() const;
    virtual bool hasMoreData() const;
    bool isFinished() const;

    void start() override;
    void finishLater();
    void abort(const QString &reason);

    virtual void feedResponse(QObject *response);

    virtual const QByteArray encodeRequest(ProtobufPluginInterface *encoder) = 0;

private:
    virtual bool begin();
    virtual bool processResponse(QObject *response);
    uint32_t m_id;
};

}
}

