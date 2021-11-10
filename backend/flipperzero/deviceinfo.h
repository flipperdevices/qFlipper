#pragma once

#include <QString>
#include <QDateTime>
#include <QSerialPortInfo>

#include "usbdeviceinfo.h"

namespace Flipper {
namespace Zero {

struct HardwareInfo {
    Q_GADGET
    Q_PROPERTY(QString version MEMBER version)
    Q_PROPERTY(QString target MEMBER target)
    Q_PROPERTY(QString body MEMBER body)
    Q_PROPERTY(QString connect MEMBER connect)

public:
    enum class Color {
        Unknown = 0,
        Black,
        White
    };

    Q_ENUM(Color)

    QString version;
    QString target;
    QString body;
    QString connect;
    Color color;

    // Needed in order to work with QVariant
    bool operator !=(const HardwareInfo &other) const { Q_UNUSED(other) return true; }
};

struct SoftwareInfo {
    Q_GADGET
    Q_PROPERTY(QString version MEMBER version)
    Q_PROPERTY(QString commit MEMBER commit)
    Q_PROPERTY(QString branch MEMBER branch)
    Q_PROPERTY(QDateTime date MEMBER date)

public:
    QString version;
    QString commit;
    QString branch;
    QDateTime date;

    // Needed in order to work with QVariant
    bool operator !=(const SoftwareInfo &other) const { Q_UNUSED(other) return true; }
};

struct StorageInfo {
    Q_GADGET
    Q_PROPERTY(int internalFree MEMBER internalFree)
    Q_PROPERTY(int externalFree MEMBER externalFree)
    Q_PROPERTY(bool isExternalPresent MEMBER isExternalPresent)
    Q_PROPERTY(bool isAssetsPresent MEMBER isAssetsPresent)

public:
    int internalFree;
    int externalFree;
    bool isExternalPresent;
    bool isAssetsPresent;

    // Needed in order to work with QVariant
    bool operator !=(const StorageInfo &other) const { Q_UNUSED(other) return true; }
};

struct DeviceInfo {
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString model MEMBER model)
    Q_PROPERTY(QString fusVersion MEMBER fusVersion)
    Q_PROPERTY(QString radioVersion MEMBER radioVersion)
    Q_PROPERTY(QString bluetoothMac MEMBER bluetoothMac)
    Q_PROPERTY(QString systemLocation MEMBER systemLocation)

    Q_PROPERTY(int batteryLevel MEMBER batteryLevel)

    Q_PROPERTY(Flipper::Zero::HardwareInfo hardware MEMBER hardware)
    Q_PROPERTY(Flipper::Zero::SoftwareInfo bootloader MEMBER bootloader)
    Q_PROPERTY(Flipper::Zero::SoftwareInfo firmware MEMBER firmware)

public:

    QString name;
    QString model;

    QString fusVersion;
    QString radioVersion;

    QString bluetoothMac;
    int batteryLevel;

    HardwareInfo hardware;
    SoftwareInfo bootloader;
    SoftwareInfo firmware;
    StorageInfo storage;

    QString systemLocation;

    USBDeviceInfo usbInfo;
    QSerialPortInfo serialInfo;
};

}
}

Q_DECLARE_METATYPE(Flipper::Zero::HardwareInfo)
Q_DECLARE_METATYPE(Flipper::Zero::SoftwareInfo)
Q_DECLARE_METATYPE(Flipper::Zero::DeviceInfo)
Q_DECLARE_METATYPE(Flipper::Zero::StorageInfo)
