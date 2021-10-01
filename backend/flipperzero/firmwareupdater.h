#pragma once

#include "abstractoperationrunner.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class CommandInterface;
class RecoveryInterface;
//class FileManagerInterface;

class FirmwareUpdater : public AbstractOperationRunner
{
    Q_OBJECT

public:
    FirmwareUpdater(DeviceState *state, QObject *parent = nullptr);

    void fullUpdate();

private slots:
    void onDeviceInfoChanged();

    void onCLIErrorOccured();
    void onRecoveryErrorOccured();

private:
    DeviceState *m_state;
    CommandInterface *m_cli;
    RecoveryInterface *m_recovery;
};

}
}

