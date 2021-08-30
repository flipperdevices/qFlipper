#pragma once

#include <QObject>

class QTimer;

class AbstractFirmwareOperation: public QObject {
    Q_OBJECT

public:
    enum BasicState {
        Idle = 0,
        Finished,
        User
    };

    explicit AbstractFirmwareOperation(QObject *parent = nullptr);
    virtual ~AbstractFirmwareOperation() {}

    virtual const QString name() const = 0;
    virtual void start() = 0;

    int state() const;
    bool isError() const;
    const QString &errorString() const;

signals:
    void finished();

protected slots:
    virtual void onOperationTimeout();

protected:
    void setState(int state);
    void setError(const QString &errorString);

    void startTimeout(int msec = 10000);
    void stopTimeout();

private:
    bool m_isError;
    QString m_errorString;
    QTimer *m_timeout;
    int m_state;
};
