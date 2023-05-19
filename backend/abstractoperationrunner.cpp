#include "abstractoperationrunner.h"

#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>

#include "abstractoperation.h"

Q_LOGGING_CATEGORY(CATEGORY_DEFAULT, "DEF")

AbstractOperationRunner::AbstractOperationRunner(QObject *parent):
    QObject(parent),
    m_state(Idle)
{}

const QLoggingCategory &AbstractOperationRunner::loggingCategory() const
{
    return CATEGORY_DEFAULT();
}

void AbstractOperationRunner::enqueueOperation(AbstractOperation *operation)
{
    if(m_state == Idle) {
        m_state = Running;
        QTimer::singleShot(0, this, &AbstractOperationRunner::processQueue);
    }

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCCritical(loggingCategory()).noquote() << operation->description() << "ERROR:" << operation->errorString();

            clearQueue();
            processQueue();

        } else {
            qCInfo(loggingCategory()).noquote() << operation->description() << "SUCCESS";
            QTimer::singleShot(0, this, &AbstractOperationRunner::processQueue);
        }

        operation->deleteLater();
    });

    m_queue.enqueue(operation);
}

void AbstractOperationRunner::processQueue()
{
    if(m_queue.isEmpty()) {
        m_state = Idle;
        return;
    }

    auto *operation = m_queue.dequeue();
    qCInfo(loggingCategory()).noquote() << operation->description() << "START";
    operation->start();
}

void AbstractOperationRunner::clearQueue()
{
    while(!m_queue.isEmpty()) {
        m_queue.dequeue()->deleteLater();
    }

    m_state = Idle;
}
