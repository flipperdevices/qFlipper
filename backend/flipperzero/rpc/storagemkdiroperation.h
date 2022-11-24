#pragma once

#include "abstractstorageoperation.h"

namespace Flipper {
namespace Zero {

class StorageMkdirOperation : public AbstractStorageOperation
{
    Q_OBJECT

public:
    StorageMkdirOperation(uint32_t id, const QByteArray &path, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;
};

}
}

