#pragma once

#include <QObject>

class QTimer;

class AbstractOperation: public QObject {
    Q_OBJECT

public:
    enum BasicState {
        Ready = 0,
        Finished,
        User
    };

    explicit AbstractOperation(QObject *parent = nullptr);
    virtual ~AbstractOperation() {}

    virtual const QString name() const = 0;
    virtual void start() = 0;
    virtual void finish() = 0;

    int state() const;
    bool isError() const;
    const QString &errorString() const;

signals:
    void started();
    void finished();

protected slots:
    virtual void onOperationTimeout();

protected:
    void setState(int state);
    void finishWithError(const QString &errorString);

    void startTimeout(int msec = 10000);
    void stopTimeout();

private:
    bool m_isError;
    QString m_errorString;
    QTimer *m_timeout;
    int m_state;
};
