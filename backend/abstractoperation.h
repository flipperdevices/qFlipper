#pragma once

#include <QObject>

#include "failable.h"

class QTimer;

class AbstractOperation: public QObject, public Failable
{
    Q_OBJECT

public:
    enum BasicState {
        Ready = 0,
        Finished,
        User
    };

    explicit AbstractOperation(QObject *parent = nullptr);
    virtual ~AbstractOperation() {}

    virtual const QString description() const = 0;
    virtual void start() = 0;
    virtual void finish() = 0;

    int operationState() const;

signals:
    void started();
    void finished();

protected slots:
    virtual void onOperationTimeout();

protected:
    void setOperationState(int state);
    void finishWithError(const QString &errorMsg);

    void startTimeout(int msec = 5000);
    void stopTimeout();

private:
    QTimer *m_timeout;
    int m_operationState;
};
