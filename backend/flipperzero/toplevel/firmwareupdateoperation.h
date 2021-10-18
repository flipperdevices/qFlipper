#pragma once

#include "abstracttopleveloperation.h"

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;
class RecoveryInterface;

class FirmwareUpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        StartingRecovery = AbstractOperation::User,
        UpdatingFirmware,
        ExitingRecovery
    };

public:
    FirmwareUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void startRecoveryMode();
    void updateFirmware();
    void exitRecoveryMode();

    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
    QFile *m_file;
};

}
}

