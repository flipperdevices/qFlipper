#pragma once

#include <QQueue>
#include <QObject>

class AbstractOperation;

class AbstractOperationRunner : public QObject
{
    Q_OBJECT

    enum State {
        Idle,
        Running
    };

    using OperationQueue = QQueue<AbstractOperation*>;

public:
    AbstractOperationRunner(QObject *parent = nullptr);

protected:
    template<class T>
    T* registerOperation(T* operation);

    virtual const QLoggingCategory &loggingCategory() const;

    void enqueueOperation(AbstractOperation *operation);

private slots:
    void processQueue();
    void clearQueue();

private:
    State m_state;
    OperationQueue m_queue;
};

template<class T>
T *AbstractOperationRunner::registerOperation(T *operation)
{
    enqueueOperation(operation);
    return operation;
}

