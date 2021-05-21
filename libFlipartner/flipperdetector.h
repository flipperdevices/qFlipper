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
    Q_PROPERTY(QList<FlipperInfo> devices READ devices NOTIFY devicesChanged)

public:
    FlipperDetector(QObject *parent = nullptr);

    const QList<FlipperInfo> devices();

signals:
    void devicesChanged(const QList<FlipperInfo>&);

private slots:
    void onDevicePluggedIn(USBDeviceParams params);
    void onDeviceUnplugged(USBDeviceParams params);

private:
    QList<FlipperInfo> m_infos;
};

#endif // FLIPPERDETECTOR_H
