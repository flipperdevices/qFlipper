#pragma once

#include <QObject>

#include "backenderror.h"
#include "flipperupdates.h"

class QAbstractListModel;

namespace Flipper {
class FlipperZero;
class DeviceRegistry;
class UpdateRegistry;

namespace Zero {
class DeviceState;
}}

class ApplicationBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BackendState backendState READ backendState NOTIFY backendStateChanged)
    Q_PROPERTY(Flipper::Zero::DeviceState* deviceState READ deviceState NOTIFY currentDeviceChanged)
    Q_PROPERTY(FirmwareUpdateState firmwareUpdateState READ firmwareUpdateState NOTIFY firmwareUpdateStateChanged)
    Q_PROPERTY(QAbstractListModel* firmwareUpdateModel READ firmwareUpdateModel CONSTANT)
    Q_PROPERTY(Flipper::Updates::VersionInfo latestFirmwareVersion READ latestFirmwareVersion NOTIFY firmwareUpdateStateChanged)
    Q_PROPERTY(BackendError::ErrorType errorType READ errorType NOTIFY errorTypeChanged)
    Q_PROPERTY(bool isQueryInProgress READ isQueryInProgress NOTIFY isQueryInProgressChanged)

public:
    enum class InputKey {
        Up,
        Down,
        Right,
        Left,
        Ok,
        Back,
    };

    Q_ENUM(InputKey)

    enum class InputType {
        Press, /* Press event, emitted after debounce */
        Release, /* Release event, emitted after debounce */
        Short, /* Short event, emitted after InputTypeRelease done withing INPUT_LONG_PRESS interval */
        Long, /* Long event, emmited after INPUT_LONG_PRESS interval, asynchronous to InputTypeRelease  */
        Repeat, /* Repeat event, emmited with INPUT_REPEATE_PRESS period after InputTypeLong event */
    };

    Q_ENUM(InputType)

    enum class BackendState {
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
        ErrorOccured = 0xff
    };

    Q_ENUM(BackendState)

    enum class FirmwareUpdateState {
        Unknown,
        Checking,
        CanUpdate,
        CanInstall,
        CanRepair,
        NoUpdates,
        ErrorOccured = 0xff
    };

    Q_ENUM(FirmwareUpdateState)

    ApplicationBackend(QObject *parent = nullptr);

    BackendState backendState() const;
    BackendError::ErrorType errorType() const;

    Flipper::FlipperZero *device() const;
    Flipper::Zero::DeviceState *deviceState() const;

    FirmwareUpdateState firmwareUpdateState() const;
    QAbstractListModel *firmwareUpdateModel() const;
    const Flipper::Updates::VersionInfo latestFirmwareVersion() const;

    bool isQueryInProgress() const;

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
    Q_INVOKABLE void sendInputEvent(int key, int type);

    Q_INVOKABLE void checkFirmwareUpdates();
    Q_INVOKABLE void finalizeOperation();

signals:
    void errorTypeChanged();
    void currentDeviceChanged();
    void backendStateChanged();
    void firmwareUpdateStateChanged();
    void isQueryInProgressChanged();

private slots:
    void onCurrentDeviceChanged();
    void onCurrentDeviceReady();
    void onDeviceOperationFinished();
    void onDeviceRegistryErrorChanged();

private:
    static void registerMetaTypes();
    static void registerComparators();

    void initConnections();

    void setBackendState(BackendState newState);
    void setErrorType(BackendError::ErrorType newErrorType);

    void waitForDeviceReady();

    Flipper::DeviceRegistry *m_deviceRegistry;
    Flipper::UpdateRegistry *m_firmwareUpdateRegistry;

    BackendState m_backendState;
    BackendError::ErrorType m_errorType;
};
