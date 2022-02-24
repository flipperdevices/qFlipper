#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class StorageStatOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    enum FileType {
        RegularFile = 0,
        Directory = 1,
        Invalid
    };

    Q_ENUM(FileType)

    StorageStatOperation(uint32_t id, const QByteArray &fileName, QObject *parent = nullptr);
    const QString description() const override;

    const QByteArray &fileName() const;
    bool hasFile() const;
    quint64 size() const;
    FileType type() const;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;

    QByteArray m_fileName;
    bool m_hasFile;
    quint64 m_size;
    FileType m_type;
};

}
}

