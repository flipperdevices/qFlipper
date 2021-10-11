#include "wirelessstackupdateoperation.h"

#include <QFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"

#include "tempdirectories.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

WirelessStackUpdateOperation::WirelessStackUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_bundleFile(new QFile(filePath, this))
{}

const QString WirelessStackUpdateOperation::description() const
{
    return QStringLiteral("Radio Stack Update @%1").arg(deviceState()->name());
}

void WirelessStackUpdateOperation::nextStateLogic()
{
    qDebug() << "+++++++++++++++++ Hello there!";
    finish();
}

void WirelessStackUpdateOperation::onSubOperationErrorOccured()
{}
