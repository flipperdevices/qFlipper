#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class SystemUpdateOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemUpdateOperation(uint32_t id, const QByteArray &manifestPath, QObject *parent = nullptr);
    const QString description() const override;
    const QString &resultString() const;
    bool isResultOk() const;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;

    QByteArray m_manifestPath;
    QString m_resultString;
    bool m_isResultOk;
};

}
}

