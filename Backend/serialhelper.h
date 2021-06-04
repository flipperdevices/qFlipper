#ifndef SERIALHELPER_H
#define SERIALHELPER_H

#include <QSerialPortInfo>

class SerialHelper
{
public:
    static QSerialPortInfo findSerialPort(const QString &serialNumber)
    {
        const auto portInfos = QSerialPortInfo::availablePorts();
        const auto it = std::find_if(portInfos.cbegin(), portInfos.cend(), [&](const QSerialPortInfo &info) {
            return info.serialNumber() == serialNumber;
        });

        if(it != portInfos.cend()) {
            return *it;
        }

        return QSerialPortInfo();
    }
};

#endif // SERIALHELPER_H
