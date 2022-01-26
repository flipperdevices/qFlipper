#pragma once

#include <QObject>

#include "failable.h"

class AbstractOperationHelper : public QObject, public Failable
{
    Q_OBJECT

public:
    enum State {
        Ready = 0,
        Finished,
        User
    };

    AbstractOperationHelper(QObject *parent = nullptr);
    virtual ~AbstractOperationHelper() {}

signals:
    void finished();

protected slots:
    void finish();
    void finishWithError(BackendError::ErrorType error, const QString &errorString);

    void advanceState();

protected:
    int state() const;
    void setState(int newState);

private:
    virtual void nextStateLogic() = 0;

    int m_state;
};
