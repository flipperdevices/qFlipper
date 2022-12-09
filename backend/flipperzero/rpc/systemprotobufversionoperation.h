#pragma once

#include "abstractprotobufoperation.h"

#include <QByteArray>

namespace Flipper {
namespace Zero {

class SystemProtobufVersionOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemProtobufVersionOperation(uint32_t id, QObject *parent = nullptr);
    const QString description() const override;

    uint32_t versionMajor() const;
    uint32_t versionMinor() const;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;

    uint32_t m_versionMajor;
    uint32_t m_versionMinor;
};

}
}

