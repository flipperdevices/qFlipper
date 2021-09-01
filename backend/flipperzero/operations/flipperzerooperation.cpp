#include "flipperzerooperation.h"

#include <QTimer>

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

Operation::Operation(FlipperZero *device, QObject *parent):
    AbstractOperation(parent),
    m_device(device)
{
    // TODO: set device persistent elsewhere
    m_device->setPersistent(true);
}

Operation::~Operation()
{
    // TODO: set device persistent elsewhere
    m_device->setPersistent(false);
}

FlipperZero *Operation::device() const
{
    return m_device;
}

void Operation::start()
{
    if(state() != Ready) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    connect(m_device, &FlipperZero::isOnlineChanged, this, &Operation::transitionToNextState);
    transitionToNextState();
}

void Operation::finish()
{
    disconnect(m_device, &FlipperZero::isOnlineChanged, this, &Operation::transitionToNextState);
    emit finished();
}
