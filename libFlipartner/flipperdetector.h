#ifndef FLIPPERDETECTOR_H
#define FLIPPERDETECTOR_H

#include <QList>
#include <QObject>
#include <QRunnable>

#include "flipperinfo.h"
#include "usbdeviceparams.h"

class FlipperDetector : public QObject
{
    Q_OBJECT

public:
    FlipperDetector(QObject *parent = nullptr);

signals:
    void flipperDetected(const FlipperInfo);
    void flipperDisconnected(const FlipperInfo);
    void flipperUpdated(const FlipperInfo);

private slots:
    void onDevicePluggedIn(USBDeviceParams params);
    void onDeviceUnplugged(USBDeviceParams params);
};

#endif // FLIPPERDETECTOR_H
