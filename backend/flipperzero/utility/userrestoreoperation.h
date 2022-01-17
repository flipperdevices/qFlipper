#pragma once

#include "abstractutilityoperation.h"

#include <QDir>
#include <QFileInfoList>

namespace Flipper {
namespace Zero {

class UserRestoreOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        ReadingBackupDir = AbstractOperation::User,
        DeletingFiles,
        WritingFiles
    };

public:
    UserRestoreOperation(CommandInterface *cli, DeviceState *deviceState, const QString &backupPath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    QDir m_backupDir;
    QByteArray m_deviceDirName;
    QFileInfoList m_files;

    bool readBackupDir();
    bool deleteFiles();
    bool writeFiles();
};

}
}

