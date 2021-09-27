#pragma once

#include "flipperzerooperation.h"

#include <QDir>

//#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class UserRestoreOperation : public Operation
{
    Q_OBJECT

public:
    UserRestoreOperation(FlipperZero *device, const QString &backupPath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void transitionToNextState() override;

private:
    QDir m_backupDir;
    QByteArray m_deviceDirName;
};

}
}

