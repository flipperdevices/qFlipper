#ifndef FLIPPERINFO_H
#define FLIPPERINFO_H

#include <QObject>

#include "usbdeviceparams.h"

struct FlipperInfo
{
    Q_GADGET
public:
    QString model;
    QString name;
    QString version;
    QString serialNumber;
    Q_PROPERTY(QString model MEMBER model)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString version MEMBER version)
    Q_PROPERTY(QString serialNumber MEMBER serialNumber)

    // TODO: get this out of here
    USBDeviceParams params;
};

Q_DECLARE_METATYPE(FlipperInfo)

#endif // FLIPPERINFO_H
