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

    Flipper::DeviceRegistry *deviceRegistry() const;
    Flipper::UpdateRegistry *firmwareUpdates() const;
    Flipper::UpdateRegistry *applicationUpdates() const;

    /* Actions available from the GUI.
     * Applies to the currently active device. */

    Q_INVOKABLE void mainAction();

    Q_INVOKABLE void createBackup();
    Q_INVOKABLE void restoreBackup();
    Q_INVOKABLE void factoryReset();

    Q_INVOKABLE void installFirmware(const QUrl &fileUrl);
    Q_INVOKABLE void installWirelessStack(const QUrl &fileUrl);
    Q_INVOKABLE void installFUS(const QUrl &fileUrl, uint32_t address);

private:
    Flipper::DeviceRegistry *m_deviceRegistry;
    Flipper::FirmwareUpdates *m_firmwareUpdates;
    Flipper::ApplicationUpdates *m_applicationUpdates;
};
