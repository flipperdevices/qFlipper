#pragma once

#include <QObject>

#include "deviceinfo.h"

namespace Flipper {

namespace Zero {
    class DeviceState;
    class ScreenStreamInterface;
    class FirmwareUpdater;
}

class FlipperZero : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Flipper::Zero::DeviceState* state READ deviceState CONSTANT)
    Q_PROPERTY(Flipper::Zero::FirmwareUpdater* updater READ updater CONSTANT)
    Q_PROPERTY(Flipper::Zero::ScreenStreamInterface* screen READ screen CONSTANT)

public:
    FlipperZero(const Zero::DeviceInfo &info, QObject *parent = nullptr);
    ~FlipperZero();

    Flipper::Zero::DeviceState *deviceState() const;
    Flipper::Zero::ScreenStreamInterface *screen() const;
    Flipper::Zero::FirmwareUpdater *updater() const;

private slots:
    void onErrorOccured();

private:
    Zero::DeviceState *m_state;
    Zero::FirmwareUpdater *m_updater;
    Zero::ScreenStreamInterface *m_screen;
};

}
