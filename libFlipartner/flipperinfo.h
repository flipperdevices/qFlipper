#ifndef FLIPPERINFO_H
#define FLIPPERINFO_H

#include <QMetaType>

#include "usbdeviceparams.h"

struct FlipperInfo
{
    struct Status
    {
        double progress = 0;
        QString message = "Update";
    };

    FlipperInfo() = default;
    FlipperInfo(const USBDeviceParams &params):
        model("Flipper Zero"), name("N/A"), target("N/A"),
        version("N/A"), params(params), status() {}

    QString model;
    QString name;
    QString target;
    QString version;

    USBDeviceParams params;
    Status status;

    bool operator ==(const FlipperInfo &other) const {
        return params.uniqueID == other.params.uniqueID;
    }

    bool operator !=(const FlipperInfo &other) const {
        return params.uniqueID != other.params.uniqueID;
    }

    bool isDFU() const {
        return params.productID == 0xdf11;
    }
};

Q_DECLARE_METATYPE(FlipperInfo)

#endif // FLIPPERINFO_H
