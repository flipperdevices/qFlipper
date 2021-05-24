#include "serialhelper.h"

QSerialPortInfo SerialHelper::findSerialPort(const FlipperInfo &info)
{
    const auto portInfos = QSerialPortInfo::availablePorts();
    for(const auto &portInfo : portInfos) {
        if(portInfo.serialNumber() == info.params.serialNumber) {
            return portInfo;
        }
    }

    return QSerialPortInfo();
}

QSerialPortInfo SerialHelper::findSerialPort(const QString &serialNumber)
{
    const auto portInfos = QSerialPortInfo::availablePorts();
    for(const auto &portInfo : portInfos) {
        if(portInfo.serialNumber() == serialNumber) {
            return portInfo;
        }
    }

    return QSerialPortInfo();
}
