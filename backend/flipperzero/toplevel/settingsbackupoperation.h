#pragma once

#include "abstracttopleveloperation.h"

#include <QElapsedTimer>

namespace Flipper {
namespace Zero {

class UtilityInterface;

class SettingsBackupOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        SavingBackup = AbstractOperation::User,
        Waiting
    };

public:
    SettingsBackupOperation(UtilityInterface *utility, DeviceState *state, const QUrl &backupDir, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void saveBackup();
    void wait();

    void onSubOperationError(AbstractOperation *operation) override;

    UtilityInterface *m_utility;
    QString m_backupDir;
    QElapsedTimer m_elapsed;
};

}
}

