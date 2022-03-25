#pragma once

#include "abstractprotobufoperation.h"
#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class StorageListOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StorageListOperation(uint32_t id, const QByteArray &path, QObject *parent = nullptr);
    const QString description() const override;
    const FileInfoList &files() const;
    bool hasPath() const;

    const QByteArray encodeRequest(ProtobufPluginInterface *encoder) override;

private:
    bool processResponse(QObject *response) override;

    QByteArray m_path;
    FileInfoList m_result;
    bool m_hasPath;
};

}
}

