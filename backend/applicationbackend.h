#pragma once

#include <QObject>

namespace Flipper {
    class DeviceRegistry;
    class UpdateRegistry;
    class FirmwareUpdates;
    class ApplicationUpdates;
}

class ApplicationBackend : public QObject
{
    Q_OBJECT

public:
    ApplicationBackend(QObject *parent = nullptr);

    Q_INVOKABLE void mainAction();

    Flipper::DeviceRegistry *deviceRegistry() const;
    Flipper::UpdateRegistry *firmwareUpdates() const;
    Flipper::UpdateRegistry *applicationUpdates() const;

private:
    Flipper::DeviceRegistry *m_deviceRegistry;
    Flipper::FirmwareUpdates *m_firmwareUpdates;
    Flipper::ApplicationUpdates *m_applicationUpdates;
};
