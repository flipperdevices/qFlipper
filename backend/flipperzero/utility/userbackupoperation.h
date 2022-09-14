#pragma once

#include "abstractutilityoperation.h"

#include <QUrl>
#include <QDir>
#include <QTemporaryDir>

#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class UserBackupOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        CreatingDirectory = AbstractOperation::User,
        GettingFileTree,
        ReadingFiles,
        CreatingArchive,
    };

public:
    UserBackupOperation(ProtobufSession *rpc, DeviceState *deviceState, const QUrl &backupUrl, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void createDirectory();
    void getFileTree();
    void readFiles();
    void createArchive();

    QUrl m_backupUrl;
    QTemporaryDir m_tempDir;
    QDir m_workDir;
    QByteArray m_deviceDirName;
    FileInfoList m_fileList;
};

}
}

