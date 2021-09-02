#pragma once

#include <QString>
#include <QDateTime>
#include <QSerialPortInfo>

#include "usbdeviceinfo.h"

namespace Flipper {
namespace Zero {

struct VersionInfo {
    QString version;
    QString commit;
    QString branch;
    QDateTime date;
};

struct DeviceInfo {
    enum class Color {
        Unknown = 0,
        Black,
        White
    };

    QString name;
    QString model;
    QString target;
    Color color;

    QString fusVersion;
    QString radioVersion;

    QString bluetoothMac;
    int batteryLevel;

    VersionInfo bootloader;
    VersionInfo firmware;

    USBDeviceInfo usbInfo;
    QSerialPortInfo serialInfo;
};

}
}
