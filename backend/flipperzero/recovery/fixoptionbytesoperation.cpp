#include "fixoptionbytesoperation.h"

#include <QIODevice>

#include "flipperzero/devicestate.h"
#include "flipperzero/recovery.h"

using namespace Flipper;
using namespace Zero;

FixOptionBytesOperation::FixOptionBytesOperation(Recovery *recovery, QIODevice *file, QObject *parent):
    AbstractRecoveryOperation(recovery, parent),
    m_file(file)
{}

FixOptionBytesOperation::~FixOptionBytesOperation()
{
    // TODO: not hide the deletion of files?
    m_file->deleteLater();
}

const QString FixOptionBytesOperation::description() const
{
    return QStringLiteral("Fix Option Bytes @%1").arg(deviceState()->name());
}

void FixOptionBytesOperation::advanceOperationState()
{
    fixOptionBytes();
}

void FixOptionBytesOperation::fixOptionBytes()
{
    if(!recovery()->downloadOptionBytes(m_file)) {
        finishWithError(recovery()->errorString());
    } else {
        finish();
    }
}
