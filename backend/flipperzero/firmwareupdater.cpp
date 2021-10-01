#include "firmwareupdater.h"

#include "devicestate.h"
#include "commandinterface.h"
#include "recoveryinterface.h"

#include "flipperzero/devicestate.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

FirmwareUpdater::FirmwareUpdater(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_state(state),
    m_cli(new CommandInterface(state, this)),
    m_recovery(new RecoveryInterface(state, this))
{
    connect(m_cli, &SignalingFailable::errorOccured, this, &FirmwareUpdater::onCLIErrorOccured);
    connect(m_recovery, &SignalingFailable::errorOccured, this, &FirmwareUpdater::onRecoveryErrorOccured);
}

void FirmwareUpdater::fullUpdate()
{
    info_msg("=========== Hello there! ==============");
//    m_cli->startRecoveryMode();
}

void FirmwareUpdater::onDeviceInfoChanged()
{
}

void FirmwareUpdater::onCLIErrorOccured()
{
    // TODO: Proper error reporting
    qDebug() << "!!! CLI ERROR !!!" << m_cli->errorString();
}

void FirmwareUpdater::onRecoveryErrorOccured()
{
    // TODO: Proper error reporting
    qDebug() << "!!! RECOVERY ERROR !!!" << m_cli->errorString();
}
