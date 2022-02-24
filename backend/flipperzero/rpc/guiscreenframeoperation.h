#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class GuiScreenFrameOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    GuiScreenFrameOperation(uint32_t id, const QByteArray &screenData, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    QByteArray m_screenData;
};

}
}

