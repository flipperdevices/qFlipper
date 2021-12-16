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
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    enum class State {
        WaitingForDevices,
        Ready,
        UpdatingDevice,
        RepairingDevice,
        CreatingBackup,
        RestoringBackup,
        FactoryResetting,
        InstallingFirmware,
        InstallingWirelessStack,
        InstallingFUS,
        OperationInterrupted
    };

    Q_ENUM(State)

    ApplicationBackend(QObject *parent = nullptr);
    State state() const;

    Flipper::DeviceRegistry *deviceRegistry() const;
    Flipper::UpdateRegistry *firmwareUpdates() const;
    Flipper::UpdateRegistry *applicationUpdates() const;

    /* Actions available from the GUI.
     * Applies to the currently active device. */

    Q_INVOKABLE void mainAction();

    Q_INVOKABLE void createBackup(const QUrl &directoryUrl);
    Q_INVOKABLE void restoreBackup(const QUrl &directoryUrl);
    Q_INVOKABLE void factoryReset();

    Q_INVOKABLE void installFirmware(const QUrl &fileUrl);
    Q_INVOKABLE void installWirelessStack(const QUrl &fileUrl);
    Q_INVOKABLE void installFUS(const QUrl &fileUrl, uint32_t address);

signals:
    void stateChanged();

private slots:
    void onDevicesChanged();
    void onFirmwareChanged();

private:
    static void registerMetaTypes();
    static void registerComparators();

    void initConnections();
    void setState(State newState);

    Flipper::DeviceRegistry *m_deviceRegistry;
    Flipper::FirmwareUpdates *m_firmwareUpdates;
    Flipper::ApplicationUpdates *m_applicationUpdates;

    State m_state;
};
