#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class SystemFactoryResetOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemFactoryResetOperation(uint32_t id, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;
};

}
}

