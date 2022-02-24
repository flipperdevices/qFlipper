#pragma once

#include "abstractprotobufoperation.h"

#include <QByteArray>

class QIODevice;

namespace Flipper {
namespace Zero {

class StorageReadOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StorageReadOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;

    QByteArray m_path;
    QIODevice *m_file;
};

}
}

