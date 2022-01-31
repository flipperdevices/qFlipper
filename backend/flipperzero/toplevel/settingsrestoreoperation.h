#pragma once

#include <QElapsedTimer>

#include "abstracttopleveloperation.h"

namespace Flipper {
namespace Zero {

class UtilityInterface;

class SettingsRestoreOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        RestoringBackup = AbstractOperation::User,
        Waiting,
        RestartingDevice
    };

public:
    SettingsRestoreOperation(UtilityInterface *utility, DeviceState *state, const QUrl &backupDir, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void restoreBackup();
    void wait();
    void restartDevice();

    void onSubOperationError(AbstractOperation *operation) override;

    UtilityInterface *m_utility;
    QString m_backupDir;
    QElapsedTimer m_elapsed;
};

}
}

