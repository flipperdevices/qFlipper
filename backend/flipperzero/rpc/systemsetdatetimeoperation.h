#pragma once

#include "abstractprotobufoperation.h"

#include <QDateTime>

namespace Flipper {
namespace Zero {

class SystemSetDateTimeOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemSetDateTimeOperation(uint32_t id, const QDateTime &dateTime, QObject *parent = nullptr);
    const QString description() const override;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    QDateTime m_dateTime;
};

}
}

