#pragma once

#include "abstractprotobufoperation.h"

#include <QHash>
#include <QByteArray>

namespace Flipper {
namespace Zero {

class PropertyGetOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    PropertyGetOperation(uint32_t id, const QByteArray &key, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray value(const QByteArray &key) const;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;

    QByteArray m_key;
    QHash<QByteArray, QByteArray> m_data;
};

}
}

