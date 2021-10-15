#include "correctoptionbytesoperation.h"

#include <QIODevice>

#include "flipperzero/devicestate.h"
#include "flipperzero/recovery.h"

using namespace Flipper;
using namespace Zero;

CorrectOptionBytesOperation::CorrectOptionBytesOperation(Recovery *recovery, QIODevice *file, QObject *parent):
    AbstractRecoveryOperation(recovery, parent),
    m_file(file)
{}

const QString CorrectOptionBytesOperation::description() const
{
    return QStringLiteral("Correct Option Bytes @%1").arg(deviceState()->name());
}

void CorrectOptionBytesOperation::advanceOperationState()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(CorrectOptionBytesOperation::CorrectingOptionBytes);
        correctOptionBytes();

    } else if(operationState() == CorrectOptionBytesOperation::CorrectingOptionBytes) {
        finish();
    } else {}
}

void CorrectOptionBytesOperation::correctOptionBytes()
{
    if(!recovery()->downloadOptionBytes(m_file)) {
        finishWithError(recovery()->errorString());
    }
}
