#include "flipperzero/flipperzerooperation.h"

#include <QTimer>

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

FlipperZeroOperation::FlipperZeroOperation(FlipperZero *device, QObject *parent):
    AbstractOperation(parent),
    m_device(device)
{
    // TODO: set device persistent elsewhere
    m_device->setPersistent(true);
}

FlipperZeroOperation::~FlipperZeroOperation()
{
    // TODO: set device persistent elsewhere
    m_device->setPersistent(false);
}

FlipperZero *FlipperZeroOperation::device() const
{
    return m_device;
}

void FlipperZeroOperation::start()
{
    if(operationState() != Ready) {
        finishWithError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    connect(m_device, &FlipperZero::isOnlineChanged, this, &FlipperZeroOperation::transitionToNextState);
    transitionToNextState();
}

void FlipperZeroOperation::finish()
{
    disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FlipperZeroOperation::transitionToNextState);
    emit finished();
}
