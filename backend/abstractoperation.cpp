#include "abstractoperation.h"

#include <QTimer>

AbstractOperation::AbstractOperation(QObject *parent):
    QObject(parent),
    m_timeoutTimer(new QTimer(this)),
    m_operationState(BasicOperationState::Ready),
    m_progress(0.0)
{
    connect(m_timeoutTimer, &QTimer::timeout, this, &AbstractOperation::onOperationTimeout);

    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(30000);
}

void AbstractOperation::finish()
{
    stopTimeout();
    setOperationState(AbstractOperation::Finished);

    emit finished();
}

int AbstractOperation::operationState() const
{
    return m_operationState;
}

void AbstractOperation::setTimeout(int msec)
{
    m_timeoutTimer->setInterval(msec);
}

double AbstractOperation::progress() const
{
    return m_progress;
}

void AbstractOperation::onOperationTimeout()
{
    finishWithError(BackendError::TimeoutError, QStringLiteral("Operation timeout (generic)"));
}

void AbstractOperation::setOperationState(int state)
{
    m_operationState = state;
}

void AbstractOperation::finishWithError(BackendError::ErrorType error, const QString &errorString)
{
    setError(error, errorString);
    finish();
}

void AbstractOperation::setProgress(double newProgress)
{
    if(qFuzzyCompare(m_progress, newProgress)) {
        return;
    }

    m_progress = newProgress;
    emit progressChanged();
}

void AbstractOperation::startTimeout()
{
    m_timeoutTimer->start();
}

void AbstractOperation::stopTimeout()
{
    m_timeoutTimer->stop();
}
