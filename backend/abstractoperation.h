#pragma once

#include <QObject>

#include "failable.h"

class QTimer;

class AbstractOperation: public QObject, public Failable
{
    Q_OBJECT

public:
    enum BasicOperationState {
        Ready = 0,
        Finished,
        User
    };

    explicit AbstractOperation(QObject *parent = nullptr);
    virtual ~AbstractOperation() {}

    virtual const QString description() const = 0;
    virtual void start() = 0;
    virtual void finish();

    int operationState() const;
    void setTimeout(int msec);

signals:
    void started();
    void finished();

protected slots:
    virtual void onOperationTimeout();

    void startTimeout();
    void stopTimeout();

protected:
    void setOperationState(int state);
    void finishWithError(BackendError::ErrorType error = BackendError::UnknownError,
                         const QString &errorString = QStringLiteral("Unknown Error"));
//    void finishWithError(const QString &errorString);

private:
    QTimer *m_timeoutTimer;
    int m_operationState;
};
