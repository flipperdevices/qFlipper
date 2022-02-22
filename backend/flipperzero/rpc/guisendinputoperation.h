#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class GuiSendInputOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    GuiSendInputOperation(uint32_t id, int key, int type, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    int m_key;
    int m_type;
};

}
}

