#pragma once

#include "abstracttopleveloperation.h"

class QFile;
class QIODevice;

namespace Flipper {
namespace Zero {

class UtilityInterface;
class RecoveryInterface;

class WirelessStackUpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        ExtractingBundle = AbstractOperation::User,
        ReadingFirmware,
        StartingRecovery,
        SettingRecoveryBootMode,
        UpdatingWirelessStack,
        SettingOSBootMode
    };

public:
    WirelessStackUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void extractBundle();
    void readFirmware();
    void startRecoveryMode();
    void setRecoveryBootMode();
    void updateWirelessStack();
    void setOSBootMode();
    void cleanup();

    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;

    QFile *m_compressedFile;
    QFile *m_uncompressedFile;

    QIODevice *m_fusFile;
    QIODevice *m_radioFile;
};

}
}

