#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class GuiStartScreenStreamOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    GuiStartScreenStreamOperation(uint32_t id, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;
};

}
}

