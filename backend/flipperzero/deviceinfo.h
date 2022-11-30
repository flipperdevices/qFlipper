#pragma once

#include <QString>
#include <QDateTime>
#include <QSerialPortInfo>

#include "deviceregion.h"
#include "devicecolor.h"
#include "usbdeviceinfo.h"

class QSerialPort;

namespace Flipper {
namespace Zero {

struct HardwareInfo {
    Q_GADGET
    Q_PROPERTY(QString version MEMBER version)
    Q_PROPERTY(QString target MEMBER target)
    Q_PROPERTY(QString body MEMBER body)
    Q_PROPERTY(QString connect MEMBER connect)

public:

    QString version;
    QString target;
    QString body;
    QString connect;
    Color color;
    Region region;

    // Needed in order to work with QVariant
    bool operator !=(const HardwareInfo &other) const { Q_UNUSED(other) return true; }
};

struct SoftwareInfo {
    Q_GADGET
    Q_PROPERTY(QString version MEMBER version)
    Q_PROPERTY(QString commit MEMBER commit)
    Q_PROPERTY(QString branch MEMBER branch)
    Q_PROPERTY(QString channel MEMBER channel)
    Q_PROPERTY(QDate date MEMBER date)

public:
    QString version;
    QString commit;
    QString branch;
    QString channel;
    QDate date;

    // Needed in order to work with QVariant
    bool operator !=(const SoftwareInfo &other) const { Q_UNUSED(other) return true; }
};

struct StorageInfo {
    Q_GADGET
    Q_PROPERTY(int internalFree MEMBER internalFree)
    Q_PROPERTY(int externalFree MEMBER externalFree)
    Q_PROPERTY(bool isExternalPresent MEMBER isExternalPresent)
    Q_PROPERTY(bool isAssetsInstalled MEMBER isAssetsInstalled)

public:
    int internalFree;
    int externalFree;
    bool isExternalPresent;
    bool isAssetsInstalled;

    // Needed in order to work with QVariant
    bool operator !=(const StorageInfo &other) const { Q_UNUSED(other) return true; }
};

struct ProtobufInfo {
public:
    uint32_t versionMajor;
    uint32_t versionMinor;
};

struct DeviceInfo {
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString model MEMBER model)
    Q_PROPERTY(QString fusVersion MEMBER fusVersion)
    Q_PROPERTY(QString radioVersion MEMBER radioVersion)
    Q_PROPERTY(QString systemLocation MEMBER systemLocation)

    Q_PROPERTY(Flipper::Zero::HardwareInfo hardware MEMBER hardware)
    Q_PROPERTY(Flipper::Zero::SoftwareInfo firmware MEMBER firmware)
    Q_PROPERTY(Flipper::Zero::StorageInfo storage MEMBER storage)

    Q_PROPERTY(int stackType MEMBER stackType)

public:
    QString name;
    QString model;

    QString fusVersion;
    QString radioVersion;
    int stackType;

    HardwareInfo hardware;
    SoftwareInfo firmware;
    ProtobufInfo protobuf;
    StorageInfo storage;

    QString systemLocation;

    USBDeviceInfo usbInfo;
    QSerialPortInfo portInfo;
};

}
}

Q_DECLARE_METATYPE(Flipper::Zero::HardwareInfo)
Q_DECLARE_METATYPE(Flipper::Zero::SoftwareInfo)
Q_DECLARE_METATYPE(Flipper::Zero::DeviceInfo)
Q_DECLARE_METATYPE(Flipper::Zero::StorageInfo)
