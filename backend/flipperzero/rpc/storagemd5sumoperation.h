#pragma once

#include "abstractstorageoperation.h"

namespace Flipper {
namespace Zero {

class StorageMd5SumOperation : public AbstractStorageOperation
{
    Q_OBJECT

public:
    StorageMd5SumOperation(uint32_t id, const QByteArray &path, QObject *parent = nullptr);

    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

    const QByteArray &md5Sum() const;

private:
    bool processResponse(QObject *response) override;
    QByteArray m_md5Sum;
};

}
}
