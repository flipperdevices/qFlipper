#ifndef SERIALHELPER_H
#define SERIALHELPER_H

#include <QThread>
#include <QSerialPortInfo>

class SerialHelper
{
public:
    static QSerialPortInfo findSerialPort(const QString &serialNumber, uint32_t numTries = 10000, uint32_t delayUs = 1000)
    {
        while(--numTries) {
            const auto portInfos = QSerialPortInfo::availablePorts();
            const auto it = std::find_if(portInfos.cbegin(), portInfos.cend(), [&](const QSerialPortInfo &info) {
                return info.serialNumber() == serialNumber;
            });

            if(it != portInfos.cend()) {
                return *it;
            }

            QThread::usleep(delayUs);
        }

        return QSerialPortInfo();
    }
};

#endif // SERIALHELPER_H
