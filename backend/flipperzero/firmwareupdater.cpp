#include "firmwareupdater.h"

#include "devicestate.h"
#include "recoveryinterface.h"
#include "utilityinterface.h"

#include "flipperzero/utility/startrecoveryoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

FirmwareUpdater::FirmwareUpdater(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_state(state),
    m_recovery(new RecoveryInterface(state, this)),
    m_utility(new UtilityInterface(state, this))
{}

void FirmwareUpdater::fullUpdate()
{
    m_state->setPersistent(true);

    auto *operation = m_utility->startRecoveryMode();
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qDebug() << "================== NAY!" << operation->errorString();
        } else {
            qDebug() << "================== YAY!";
        }

        m_state->setPersistent(false);
    });
}
