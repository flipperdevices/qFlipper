#pragma once

#include "abstractutilityoperation.h"

#include <QUrl>
#include <QDir>
#include <QFileInfoList>
#include <QTemporaryDir>

namespace Flipper {
namespace Zero {

class UserRestoreOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        UncompressingArchive = AbstractOperation::User,
        ReadingBackupDir,
        DeletingFiles,
        WritingFiles
    };

public:
    UserRestoreOperation(ProtobufSession *rpc, DeviceState *deviceState, const QUrl &backupUrl, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    QUrl m_backupUrl;
    QTemporaryDir m_tempDir;
    QDir m_workDir;
    QByteArray m_remoteDirName;
    QFileInfoList m_files;

    void uncompressArchive();
    void readBackupDir();
    void deleteFiles();
    void writeFiles();
};

}
}

