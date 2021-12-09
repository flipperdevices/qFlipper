#include "abstractoperationrunner.h"

#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>

#include "abstractoperation.h"

#define CALL_LATER(obj, func) (QTimer::singleShot(0, obj, func))

Q_LOGGING_CATEGORY(CATEGORY_DEFAULT, "DEFAULT")

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

const QLoggingCategory &AbstractOperationRunner::loggingCategory() const
{
    return CATEGORY_DEFAULT();
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
        if(m_state == State::Finishing) {
            m_state = State::Idle;
            return;
        }

        m_state = State::Finishing;

        if(!onQueueFinished()) {
            setError(QStringLiteral("Failed to finish the operation queue"));
            return;

        } else if(m_queue.isEmpty()) { //onQueueFinished() might add additional operations to the queue
            m_state = State::Idle;
            return;
        }
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

        qCInfo(loggingCategory()).noquote() << operation->description() << (operation->isError() ? QStringLiteral("ERROR: ") + operation->errorString() : QStringLiteral("SUCCESS"));

        onOperationFinished(operation);
        operation->deleteLater();
    });

    qCInfo(loggingCategory()).noquote() << operation->description() << "START";

    onOperationStarted(operation);
    operation->start();
}

void AbstractOperationRunner::clearQueue()
{
    while(!m_queue.isEmpty()) {
        m_queue.dequeue()->deleteLater();
    }
}
