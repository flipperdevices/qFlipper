#pragma once

#include "abstractprotobufoperation.h"

#include <QDateTime>

namespace Flipper {
namespace Zero {

class SystemGetDateTimeOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemGetDateTimeOperation(uint32_t id, QObject *parent = nullptr);
    const QString description() const override;
    const QDateTime &dateTime() const;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;

    QDateTime m_dateTime;
};

}
}

