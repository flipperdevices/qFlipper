#ifndef SERIALFINDER_H
#define SERIALFINDER_H

#include <QObject>
#include <QThread>
#include <QSerialPortInfo>

class QTimer;

class SerialFinder : public QObject
{
    Q_OBJECT

public:
    SerialFinder(const QString &serialNumber, QObject *parent = nullptr);

    // TODO get rid of this method
    static QSerialPortInfo findSerialPort(const QString &serialNumber, uint32_t numTries = 100, uint32_t delayMs = 15)
    {
        while(--numTries) {
            const auto portInfos = QSerialPortInfo::availablePorts();
            const auto it = std::find_if(portInfos.cbegin(), portInfos.cend(), [&](const QSerialPortInfo &info) {
                return info.serialNumber() == serialNumber;
            });

            if(it != portInfos.cend()) {
                return *it;
            }

            QThread::msleep(delayMs);
        }

        return QSerialPortInfo();
    }

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
