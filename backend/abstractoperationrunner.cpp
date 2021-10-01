#include "abstractoperationrunner.h"

#include <QTimer>

#include "abstractoperation.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

AbstractOperationRunner::AbstractOperationRunner(QObject *parent):
    SignalingFailable(parent),
    m_state(State::Idle)
{}

bool AbstractOperationRunner::onQueueStarted()
{
    //Default empty implementation
    return true;
}

bool AbstractOperationRunner::onQueueFinished()
{
    //Default empty implementation
    return true;
}

void AbstractOperationRunner::onOperationStarted(AbstractOperation *operation)
{
    Q_UNUSED(operation);
    //Default empty implementation
}

void AbstractOperationRunner::onOperationFinished(AbstractOperation *operation)
{
    Q_UNUSED(operation);
    //Default empty implementation
}

void AbstractOperationRunner::enqueueOperation(AbstractOperation *operation)
{
    if(m_state == State::Idle) {
        m_state = State::Running;

        if(!onQueueStarted()) {
            setError(QStringLiteral("Failed to start the operation queue"));
            return;
        }

        CALL_LATER(this, &AbstractOperationRunner::processQueue);
    }

    m_queue.enqueue(operation);
}

void AbstractOperationRunner::processQueue()
{
    if(m_queue.isEmpty()) {
        m_state = State::Idle;

        if(!onQueueFinished()) {
            setError(QStringLiteral("Failed to finish the operation queue"));
        }

        return;
    }

    auto *operation = m_queue.dequeue();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            setError(operation->errorString());

            clearQueue();
            processQueue();

        } else {
            CALL_LATER(this, &AbstractOperationRunner::processQueue);
        }

        onOperationFinished(operation);
        operation->deleteLater();
    });

    onOperationFinished(operation);
    operation->start();
}

void AbstractOperationRunner::clearQueue()
{
    while(!m_queue.isEmpty()) {
        m_queue.dequeue()->deleteLater();
    }
}
