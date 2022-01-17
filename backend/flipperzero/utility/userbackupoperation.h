#pragma once

#include "abstractutilityoperation.h"

#include <QDir>

#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class UserBackupOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        CreatingDirectory = AbstractOperation::User,
        GettingFileTree,
        ReadingFiles
    };

public:
    UserBackupOperation(CommandInterface *cli, DeviceState *deviceState, const QString &backupPath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void createBackupDirectory();
    void getFileTree();
    void readFiles();

    QDir m_backupDir;
    QByteArray m_deviceDirName;
    FileInfoList m_fileList;
};

}
}

