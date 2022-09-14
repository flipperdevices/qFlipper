#pragma once

#include "abstracttopleveloperation.h"

#include <QUrl>

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;
class RecoveryInterface;

class FirmwareInstallOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        SavingBackup = AbstractOperation::User,
        StartingRecovery,
        InstallingFirmware,
        ExitingRecovery,
        RestoringBackup,
        RestartingDevice
    };

public:
    FirmwareInstallOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void saveBackup();
    void startRecovery();
    void installFirmware();
    void exitRecovery();
    void restoreBackup();
    void restartDevice();

    void onSubOperationError(AbstractOperation *operation) override;

    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
    QFile *m_file;
    bool m_skipBackup;
    QUrl m_backupUrl;
};

}
}

