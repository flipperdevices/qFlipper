#include "factoryresetoperation.h"

#include <QTimer>

#include "flipperzero/devicestate.h"
#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/factoryresetutiloperation.h"

static constexpr qint64 MINIMUM_OPERATION_TIME_MS = 2000;

using namespace Flipper;
using namespace Zero;

FactoryResetOperation::FactoryResetOperation(UtilityInterface *utility, DeviceState *state, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_utility(utility)
{}

const QString FactoryResetOperation::description() const
{
    return QStringLiteral("Factory reset (Toplevel) @%1").arg(deviceState()->name());
}

void FactoryResetOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(FactoryResetOperation::ResettingDevice);
        resetDevice();

    } else if(operationState() == FactoryResetOperation::ResettingDevice) {
        setOperationState(FactoryResetOperation::Waiting);
        wait();

    } else if(operationState() == FactoryResetOperation::Waiting) {
        finish();
    }
}

void FactoryResetOperation::resetDevice()
{
    m_elapsed.start();
    registerSubOperation(m_utility->factoryReset());
}

void FactoryResetOperation::wait()
{
    const auto delay = qMax<qint64>(MINIMUM_OPERATION_TIME_MS - m_elapsed.elapsed(), 0);
    QTimer::singleShot(delay, this, &FactoryResetOperation::advanceOperationState);
}

