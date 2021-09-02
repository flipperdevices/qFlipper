#ifndef SERIALFINDER_H
#define SERIALFINDER_H

#include <QObject>
#include <QSerialPortInfo>

class QTimer;

class SerialFinder : public QObject
{
    Q_OBJECT

public:
    SerialFinder(const QString &serialNumber, QObject *parent = nullptr);

    void setNumberOfTries(int numTries);
    void setTryPeriod(int periodMs);

signals:
    void finished(const QSerialPortInfo&);

private slots:
    void findMatchingPort();

private:
    QTimer *m_timer;
    QString m_serialNumber;

    int m_numTries;
    int m_periodMs;
};

#endif // SERIALFINDER_H
