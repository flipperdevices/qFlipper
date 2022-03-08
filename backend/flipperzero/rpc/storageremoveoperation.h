#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class StorageRemoveOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StorageRemoveOperation(uint32_t id, const QByteArray &path, bool recursive, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    QByteArray m_path;
    bool m_recursive;
};

}
}

