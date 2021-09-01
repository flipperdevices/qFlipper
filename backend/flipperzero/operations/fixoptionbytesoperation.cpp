#include "fixoptionbytesoperation.h"

#include <QIODevice>

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
    m_file->deleteLater();
}

const QString FixOptionBytesOperation::name() const
{
    return QString("Fix Option Bytes @%1 %2").arg(m_device->model(), m_device->name());
}

void FixOptionBytesOperation::start()
{
    if(state() != Ready) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    connect(m_device, &FlipperZero::isOnlineChanged, this, &FixOptionBytesOperation::transitionToNextState);
    transitionToNextState();
}

void FixOptionBytesOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Ready) {
        setState(State::BootingToDFU);
        bootToDFU();

    } else if(state() == State::BootingToDFU) {
        setState(FixOptionBytesOperation::FixingOptionBytes);
        fixOptionBytes();

    } else if(state() == State::FixingOptionBytes) {
        setState(AbstractFirmwareOperation::Finished);
        finish();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void FixOptionBytesOperation::onOperationTimeout()
{
    switch(state()) {
    case FixOptionBytesOperation::BootingToDFU:
        setError(QStringLiteral("Failed to reach DFU mode: Operation timeout."));
        break;
    case FixOptionBytesOperation::FixingOptionBytes:
        setError(QStringLiteral("Failed to write the corrected Option Bytes: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void FixOptionBytesOperation::bootToDFU()
{
    if(m_device->isDFU()) {
        transitionToNextState();
    } else if(!m_device->bootToDFU()) {
        setError(QStringLiteral("Failed to enter DFU mode."));
    } else {}
}

void FixOptionBytesOperation::fixOptionBytes()
{
    if(!m_device->downloadOptionBytes(m_file)) {
        setError("Failed to write corrected Option Bytes.");
    }
}

void FixOptionBytesOperation::finish()
{
    disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FixOptionBytesOperation::transitionToNextState);
    emit finished();
}
