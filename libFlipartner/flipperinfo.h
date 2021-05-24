#ifndef FLIPPERINFO_H
#define FLIPPERINFO_H

#include <QMetaType>
#include "usbdeviceparams.h"

struct FlipperInfo
{
//    FlipperInfo() = default;
//    FlipperInfo(const USBDeviceParams &params): params(params) {}
//    FlipperInfo(const QString &model, const QString &name, const QString &version):
//        model(model), name(name), version(version), params() {}
//    FlipperInfo(const QString &model, const QString &name, const QString &version, const USBDeviceParams &params):
//        model(model), name(name), version(version), params(params) {}

    QString model;
    QString name;
    QString target;
    QString version;

    // TODO: get this out of here
    USBDeviceParams params;

    bool operator ==(const FlipperInfo &other) const {
        return params.uniqueID == other.params.uniqueID;
    }

    bool isDFU() const {
        return params.vendorID == 0xdf11;
    }
};

Q_DECLARE_METATYPE(FlipperInfo)

#endif // FLIPPERINFO_H
