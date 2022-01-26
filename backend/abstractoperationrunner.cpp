#include "abstractoperationrunner.h"

#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>

#include "abstractoperation.h"

Q_LOGGING_CATEGORY(CATEGORY_DEFAULT, "DEFAULT")

AbstractOperationRunner::AbstractOperationRunner(QObject *parent):
    QObject(parent),
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
        QTimer::singleShot(0, this, &AbstractOperationRunner::processQueue);
    }

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
            qCCritical(loggingCategory()).noquote() << operation->description() << "ERROR:" << operation->errorString();

            setError(operation->errorString());

            clearQueue();
            processQueue();

        } else {
            qCInfo(loggingCategory()).noquote() << operation->description() << "SUCCESS";
            QTimer::singleShot(0, this, &AbstractOperationRunner::processQueue);
        }

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
