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

const QString FixOptionBytesOperation::description() const
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
        finishWithError(QStringLiteral("Unexpected state."));
        device()->setError(errorString());
    }
}

void FixOptionBytesOperation::onOperationTimeout()
{
    QString msg;

    if(state() == FixOptionBytesOperation::BootingToDFU) {
        msg = QStringLiteral("Failed to reach DFU mode: Operation timeout.");
    } else if(state() == FixOptionBytesOperation::FixingOptionBytes) {
        msg = QStringLiteral("Failed to write the corrected Option Bytes: Operation timeout.");
    } else {
        msg = QStringLiteral("Should not have timed out here, probably a bug.");
    }

    finishWithError(msg);
    device()->setError(errorString());
}

void FixOptionBytesOperation::bootToDFU()
{
    if(device()->isDFU()) {
        transitionToNextState();
    } else if(!device()->bootToDFU()) {
        finishWithError(device()->errorString());
    } else {}
}

void FixOptionBytesOperation::fixOptionBytes()
{
    if(!device()->recovery()->downloadOptionBytes(m_file)) {
        finishWithError(device()->errorString());
    }
}
