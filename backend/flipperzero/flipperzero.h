#pragma once

#include <QObject>

#include "deviceinfo.h"

namespace Flipper {

namespace Zero {
    class DeviceState;
    class CommandInterface;
    class ScreenStreamer;
    class FirmwareUpdater;
}

class FlipperZero : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Flipper::Zero::DeviceState* state READ deviceState CONSTANT)
    Q_PROPERTY(Flipper::Zero::FirmwareUpdater* updater READ updater CONSTANT)
    Q_PROPERTY(Flipper::Zero::ScreenStreamer* streamer READ streamer CONSTANT)

public:
    FlipperZero(const Zero::DeviceInfo &info, QObject *parent = nullptr);
    ~FlipperZero();

    Flipper::Zero::DeviceState *deviceState() const;
    Flipper::Zero::ScreenStreamer *streamer() const;
    Flipper::Zero::FirmwareUpdater *updater() const;

private slots:
    void onStreamConditionChanged();
    void onUpdaterErrorOccured();

private:
    Zero::DeviceState *m_state;
    Zero::CommandInterface *m_rpc;
    Zero::FirmwareUpdater *m_updater;
    Zero::ScreenStreamer *m_streamer;
};

}
