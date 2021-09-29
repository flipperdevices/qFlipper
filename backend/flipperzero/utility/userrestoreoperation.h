#pragma once

#include "flipperzero/flipperzerooperation.h"

#include <QDir>
#include <QFileInfoList>

namespace Flipper {
namespace Zero {

class UserRestoreOperation : public FlipperZeroOperation
{
    Q_OBJECT

    enum State {
        ReadingBackupDir = BasicState::User,
        DeletingFiles,
        WritingFiles
    };

public:
    UserRestoreOperation(FlipperZero *device, const QString &backupPath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void transitionToNextState() override;

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

