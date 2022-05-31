#pragma once

#include "abstractprotobufoperation.h"

#include <QByteArray>

class QIODevice;

namespace Flipper {
namespace Zero {

class StorageWriteOperation : public AbstractProtobufOperation
{
    Q_OBJECT

    enum RequestType {
        StorageWrite,
        StatusPing
    };

public:
    StorageWriteOperation(uint32_t id, const QByteArray &path, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;
    bool hasMoreData() const override;
    void feedResponse(QObject *response) override;
    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool begin() override;
    bool processResponse(QObject *response) override;

    QByteArray m_path;
    QIODevice *m_file;

    RequestType m_subRequest;
    qint64 m_chunksPerPing;
    qint64 m_chunksWritten;
    double m_percentPerPing;
};

}
}

