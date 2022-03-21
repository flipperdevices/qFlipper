#pragma once

#include <QDir>
#include <QFileInfoList>

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class DirectoryUploadOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        ReadingLocalDir = AbstractOperation::User,
        CreatingRemoteDir,
        WritingFiles
    };

public:
    DirectoryUploadOperation(ProtobufSession *rpc, DeviceState *deviceState, const QString &localDir,
                             const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void readLocalDir();
    void createRemoteDir();
    void writeFiles();

    QDir m_localDir;
    QByteArray m_remotePath;
    QFileInfoList m_files;
};

}
}

