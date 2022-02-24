#pragma once

#include "abstractprotobufoperation.h"

#include <QByteArray>

class QIODevice;

namespace Flipper {
namespace Zero {

class StorageWriteOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StorageWriteOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;
    bool hasMoreData() const override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    QByteArray m_path;
    QIODevice *m_file;
};

}
}

