#include "fixoptionbytesoperation.h"

#include <QIODevice>

#include "flipperzero/flipperzero.h"
#include "flipperzero/recoverycontroller.h"

using namespace Flipper;
using namespace Zero;

FixOptionBytesOperation::FixOptionBytesOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    Operation(device, parent),
    m_file(file)
{
    device->setMessage(QStringLiteral("Fix Option Bytes operation pending..."));
}

FixOptionBytesOperation::~FixOptionBytesOperation()
{
    m_file->deleteLater();
}

const QString FixOptionBytesOperation::name() const
{
    return QStringLiteral("Fix Option Bytes @%1 %2").arg(device()->model(), device()->name());
}

void FixOptionBytesOperation::transitionToNextState()
{
    if(!device()->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(state() == AbstractOperation::Ready) {
        setState(State::BootingToDFU);
        bootToDFU();

    } else if(state() == State::BootingToDFU) {
        setState(FixOptionBytesOperation::FixingOptionBytes);
        fixOptionBytes();

    } else if(state() == State::FixingOptionBytes) {
        setState(AbstractOperation::Finished);
        finish();

    } else {
        setError(QStringLiteral("Unexpected state."));
        device()->setError(errorString());
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

    device()->setError(errorString());
}

void FixOptionBytesOperation::bootToDFU()
{
    if(device()->isDFU()) {
        transitionToNextState();
    } else if(!device()->bootToDFU()) {
        setError(device()->recovery()->errorString());
    } else {}
}

void FixOptionBytesOperation::fixOptionBytes()
{
    if(!device()->recovery()->downloadOptionBytes(m_file)) {
        setError(device()->recovery()->errorString());
    }
}
