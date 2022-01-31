#pragma once

#include "abstracttopleveloperation.h"

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;
class RecoveryInterface;

class AbstractCore2UpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        StartingRecovery = AbstractOperation::User,
        SettingRecoveryBootMode,
        UpdatingWirelessStack,
        SettingOSBootMode
    };

public:
    AbstractCore2UpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent = nullptr);

private slots:
    void nextStateLogic() override;

private:
    virtual void updateCore2Firmware() = 0;

    void startRecoveryMode();
    void setRecoveryBootMode();
    void setOSBootMode();

    void onSubOperationError(AbstractOperation *operation) override;

protected:
    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
    QFile *m_file;
};

class WirelessStackUpdateOperation : public AbstractCore2UpdateOperation
{
    Q_OBJECT

public:
    WirelessStackUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent = nullptr);
    const QString description() const override;

private:
    void updateCore2Firmware() override;
};

class FUSUpdateOperation : public AbstractCore2UpdateOperation
{
    Q_OBJECT

public:
    FUSUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, uint32_t address, QObject *parent = nullptr);
    const QString description() const override;

private:
    void updateCore2Firmware() override;
    uint32_t m_address;
};

}
}

