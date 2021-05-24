#ifndef SERIALHELPER_H
#define SERIALHELPER_H

#include <QSerialPortInfo>

#include "flipperinfo.h"

class SerialHelper
{
public:
    static QSerialPortInfo findSerialPort(const FlipperInfo &info);
    static QSerialPortInfo findSerialPort(const QString &serialNumber);
};

#endif // SERIALHELPER_H
