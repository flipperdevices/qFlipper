#pragma once

#include "signalingfailable.h"

#include <QQueue>

class AbstractOperation;

class AbstractOperationRunner : public SignalingFailable
{
    Q_OBJECT

    enum class State {
        Idle,
        Running
    };

    using OperationQueue = QQueue<AbstractOperation*>;

public:
    AbstractOperationRunner(QObject *parent = nullptr);

protected:
    virtual bool onQueueStarted();
    virtual bool onQueueFinished();

    virtual void onOperationStarted(AbstractOperation *operation);
    virtual void onOperationFinished(AbstractOperation *operation);

    virtual const QLoggingCategory &loggingCategory() const;

    void enqueueOperation(AbstractOperation *operation);

private slots:
    void processQueue();
    void clearQueue();

private:
    State m_state;
    OperationQueue m_queue;
};

