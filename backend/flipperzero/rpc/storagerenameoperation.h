#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class StorageRenameOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StorageRenameOperation(uint32_t id, const QByteArray &oldPath, const QByteArray &newPath, QObject *parent = nullptr);
    const QString description() const override;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    QByteArray m_oldPath;
    QByteArray m_newPath;
};

}
}

