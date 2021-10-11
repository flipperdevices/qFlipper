#pragma once

#include "abstracttopleveloperation.h"

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;
class RecoveryInterface;

class WirelessStackUpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        StartingRecovery = AbstractOperation::User,
        SettingDFUBootMode,
        DownloadingWirelessStack
    };

public:
    WirelessStackUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent = nullptr);

    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void onSubOperationErrorOccured() override;

    void startRecoveryMode();
    void setBootMode();
    void downloadWirelessStack();

    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
    QFile *m_bundleFile;
};

}
}

