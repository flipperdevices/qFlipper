#pragma once

#include "flipperzerooperation.h"

#include <QDir>

#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class UserBackupOperation : public Operation
{
    Q_OBJECT

    enum State {
        CreatingDirectory = BasicState::User,
        GettingFileTree,
        ReadingFiles
    };

public:
    UserBackupOperation(FlipperZero *device, const QString &backupPath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void transitionToNextState() override;

private:
    bool createBackupDirectory();
    bool readFiles();

    QDir m_backupDir;
    QByteArray m_deviceDirName;
    FileInfoList m_fileList;
};

}
}

