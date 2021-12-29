#pragma once

#include <QObject>

namespace Flipper {
class FlipperZero;
class DeviceRegistry;
class UpdateRegistry;
class FirmwareUpdates;
}

class ApplicationBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(UpdateStatus updateStatus READ updateStatus NOTIFY updateStatusChanged)
    Q_PROPERTY(Flipper::FlipperZero* currentDevice READ currentDevice NOTIFY currentDeviceChanged)

public:
    enum class State {
        WaitingForDevices,
        Ready,
        ScreenStreaming,
        UpdatingDevice,
        RepairingDevice,
        CreatingBackup,
        RestoringBackup,
        FactoryResetting,
        InstallingFirmware,
        InstallingWirelessStack,
        InstallingFUS,
        Finished,
        ErrorOccured
    };

    Q_ENUM(State)

    enum class UpdateStatus {
        Unknown,
        CanUpdate,
        CanInstall,
        CanRepair,
        NoUpdates
    };

    Q_ENUM(UpdateStatus)

    ApplicationBackend(QObject *parent = nullptr);

    State state() const;
    UpdateStatus updateStatus() const;

    Flipper::FlipperZero *currentDevice() const;
    Flipper::UpdateRegistry *firmwareUpdates() const;

    /* Actions available from the GUI.
     * Applies to the currently active device. */

    Q_INVOKABLE void mainAction();

    Q_INVOKABLE void createBackup(const QUrl &directoryUrl);
    Q_INVOKABLE void restoreBackup(const QUrl &directoryUrl);
    Q_INVOKABLE void factoryReset();

    Q_INVOKABLE void installFirmware(const QUrl &fileUrl);
    Q_INVOKABLE void installWirelessStack(const QUrl &fileUrl);
    Q_INVOKABLE void installFUS(const QUrl &fileUrl, uint32_t address);

    Q_INVOKABLE void startFullScreenStreaming();
    Q_INVOKABLE void stopFullScreenStreaming();

    Q_INVOKABLE void finalizeOperation();

signals:
    void stateChanged();
    void updateStatusChanged();
    void currentDeviceChanged();

private slots:
    void onCurrentDeviceChanged();
    void onDeviceOperationFinished();

private:
    static void registerMetaTypes();
    static void registerComparators();

    void initConnections();

    void setState(State newState);

    Flipper::DeviceRegistry *m_deviceRegistry;
    Flipper::FirmwareUpdates *m_firmwareUpdates;

    State m_state;
    UpdateStatus m_updateStatus;
};
