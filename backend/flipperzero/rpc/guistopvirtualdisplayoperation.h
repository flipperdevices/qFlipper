#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class GuiStopVirtualDisplayOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    GuiStopVirtualDisplayOperation(uint32_t id, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;
};

}
}

