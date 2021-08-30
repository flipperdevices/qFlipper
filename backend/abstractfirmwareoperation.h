#pragma once

#include <QObject>

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

protected:
    void setState(int state);
    void setError(const QString &errorString);

private:
    bool m_isError;
    QString m_errorString;
    int m_state;
};
