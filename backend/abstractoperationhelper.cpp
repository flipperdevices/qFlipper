#include "abstractoperationhelper.h"

#include <QTimer>

AbstractOperationHelper::AbstractOperationHelper(QObject *parent):
    QObject(parent),
    m_state(State::Ready)
{
    advanceState();
}

void AbstractOperationHelper::finish()
{
    setState(State::Finished);
    emit finished();
}

void AbstractOperationHelper::finishWithError(const QString &errorMessage)
{
    setErrorString(errorMessage);
    finish();
}

void AbstractOperationHelper::advanceState()
{
    QTimer::singleShot(0, this, &AbstractOperationHelper::nextStateLogic);
}

int AbstractOperationHelper::state() const
{
    return m_state;
}

void AbstractOperationHelper::setState(int newState)
{
    m_state = newState;
}
