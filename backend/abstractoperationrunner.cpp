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

const QLoggingCategory &AbstractOperationRunner::loggingCategory() const
{
    return CATEGORY_DEFAULT();
}

void AbstractOperationRunner::enqueueOperation(AbstractOperation *operation)
{
    if(m_state == State::Idle) {
        m_state = State::Running;
        CALL_LATER(this, &AbstractOperationRunner::processQueue);
    }

    qDebug(loggingCategory()) << "----- Enqueued operation:" << operation->description();
    m_queue.enqueue(operation);
}

void AbstractOperationRunner::processQueue()
{
    if(m_queue.isEmpty()) {
            m_state = State::Idle;
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

        qCInfo(loggingCategory()).noquote() << operation->description() << (operation->isError() ? QStringLiteral("ERROR: ") + operation->errorString() : QStringLiteral("SUCCESS"));

        operation->deleteLater();
    });

    qCInfo(loggingCategory()).noquote() << operation->description() << "START";

    operation->start();
}

void AbstractOperationRunner::clearQueue()
{
    while(!m_queue.isEmpty()) {
        m_queue.dequeue()->deleteLater();
    }
}
