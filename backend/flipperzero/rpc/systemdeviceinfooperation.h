#pragma once

#include "abstractprotobufoperation.h"

#include <QHash>
#include <QByteArray>

namespace Flipper {
namespace Zero {

class SystemDeviceInfoOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemDeviceInfoOperation(uint32_t id, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray value(const QByteArray &key) const;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;
    QHash<QByteArray, QByteArray> m_data;
};

}
}

