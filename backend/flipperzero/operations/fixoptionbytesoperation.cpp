#include "fixoptionbytesoperation.h"

#include <QIODevice>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

FixOptionBytesOperation::FixOptionBytesOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    AbstractFirmwareOperation(parent),
    m_device(device),
    m_file(file)
{
    m_device->setPersistent(true);
    m_device->setStatusMessage(QObject::tr("Fix boot issues operation pending..."));
}

FixOptionBytesOperation::~FixOptionBytesOperation()
{
    m_device->setPersistent(false);
}

const QString FixOptionBytesOperation::name() const
{
    return QString("Fix Option Bytes @%1 %2").arg(m_device->model(), m_device->name());
}

void FixOptionBytesOperation::start()
{
    if(state() != Idle) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    transitionToNextState();
}

void FixOptionBytesOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Idle) {
        setState(State::WaitingForDFU);
        connect(m_device, &FlipperZero::isOnlineChanged, this, &FixOptionBytesOperation::transitionToNextState);
        doEnterDFUMode();

    } else if(state() == State::WaitingForDFU) {
        setState(FixOptionBytesOperation::WaitingForFirmwareBoot);
        doFixOptionBytes();

    } else if(state() == State::WaitingForFirmwareBoot) {
        setState(AbstractFirmwareOperation::Finished);
        disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FixOptionBytesOperation::transitionToNextState);
        emit finished();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void FixOptionBytesOperation::onOperationTimeout()
{
    switch(state()) {
    case FixOptionBytesOperation::WaitingForDFU:
        setError(QStringLiteral("Failed to reach DFU mode: Operation timeout."));
        break;
    case FixOptionBytesOperation::WaitingForFirmwareBoot:
        setError(QStringLiteral("Failed to write the corrected Option Bytes: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void FixOptionBytesOperation::doEnterDFUMode()
{
    if(m_device->isDFU()) {
        transitionToNextState();
    } else if(!m_device->enterDFU()) {
        setError(QStringLiteral("Failed to enter DFU mode."));
        return;
    } else {
        startTimeout();
    }
}

void FixOptionBytesOperation::doFixOptionBytes()
{
    if(!m_device->fixOptionBytes(m_file)) {
        setError("Failed to write corrected Option Bytes.");
    } else {
        startTimeout();
    }
}
