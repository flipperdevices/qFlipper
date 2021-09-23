#pragma once

#include <QObject>

#include "flipperzerooperation.h"

namespace Flipper {

class FlipperZero;

namespace Zero {

class UserBackupOperation : public Operation
{
    Q_OBJECT

    enum State {

    };

public:
    UserBackupOperation(FlipperZero *device, const QString &path, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void transitionToNextState() override;
    void onOperationTimeout() override;

private:
    QString m_backupPath;
};

}
}

