#pragma once

#include "abstractutilityoperation.h"

#include <QDir>

#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class DirectoryDownloadOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        CreatingDirectory = AbstractOperation::User,
        GettingFileTree,
        ReadingFiles
    };

public:
    DirectoryDownloadOperation(ProtobufSession *rpc, DeviceState *deviceState, const QString &targetPath,
                               const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void createLocalDirectory();
    void getFileTree();
    void readFiles();

    QDir m_targetDir;
    QByteArray m_remotePath;
    FileInfoList m_fileList;
    qint64 m_totalSize;
};

}
}

