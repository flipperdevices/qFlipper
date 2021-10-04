#pragma once

#include "abstractoperationrunner.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class RecoveryInterface;
class UtilityInterface;

class FirmwareUpdater : public AbstractOperationRunner
{
    Q_OBJECT

public:
    FirmwareUpdater(DeviceState *state, QObject *parent = nullptr);

    void fullUpdate();

private slots:

private:
    DeviceState *m_state;
    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
};

}
}

