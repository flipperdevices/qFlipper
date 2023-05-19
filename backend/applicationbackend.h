#pragma once

#include <QObject>
#include <QAbstractListModel>

#include "backenderror.h"
#include "flipperupdates.h"

namespace Flipper {
class FlipperZero;
class DeviceRegistry;
class UpdateRegistry;

namespace Zero {
class DeviceState;
class FileManager;
class ScreenStreamer;
class VirtualDisplay;
}}

#if QT_VERSION >= 0x060000
Q_MOC_INCLUDE("flipperzero/devicestate.h")
Q_MOC_INCLUDE("flipperzero/screenstreamer.h")
Q_MOC_INCLUDE("flipperzero/virtualdisplay.h")
Q_MOC_INCLUDE("flipperzero/filemanager.h")
#endif

class ApplicationBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BackendState backendState READ backendState NOTIFY backendStateChanged)
    Q_PROPERTY(Flipper::Zero::DeviceState* deviceState READ deviceState NOTIFY currentDeviceChanged)
    Q_PROPERTY(Flipper::Zero::ScreenStreamer* screenStreamer READ screenStreamer CONSTANT)
    Q_PROPERTY(Flipper::Zero::VirtualDisplay* virtualDisplay READ virtualDisplay CONSTANT)
    Q_PROPERTY(Flipper::Zero::FileManager* fileManager READ fileManager CONSTANT)
    Q_PROPERTY(FirmwareUpdateState firmwareUpdateState READ firmwareUpdateState NOTIFY firmwareUpdateStateChanged)
    Q_PROPERTY(QAbstractListModel* firmwareUpdateModel READ firmwareUpdateModel CONSTANT)
    Q_PROPERTY(Flipper::Updates::VersionInfo latestFirmwareVersion READ latestFirmwareVersion NOTIFY firmwareUpdateStateChanged)
    Q_PROPERTY(BackendError::ErrorType errorType READ errorType NOTIFY errorTypeChanged)
    Q_PROPERTY(bool isQueryInProgress READ isQueryInProgress NOTIFY isQueryInProgressChanged)

public:
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

    Flipper::DeviceRegistry *deviceRegistry() const;

    Flipper::Zero::ScreenStreamer *screenStreamer() const;
    Flipper::Zero::VirtualDisplay *virtualDisplay() const;
    Flipper::Zero::FileManager *fileManager() const;

    FirmwareUpdateState firmwareUpdateState() const;
    QAbstractListModel *firmwareUpdateModel() const;
    const Flipper::Updates::VersionInfo latestFirmwareVersion() const;

    // TODO: Replace it with a state
    bool isQueryInProgress() const;

    /* Actions available from the GUI.
     * Applies to the currently active device. */

    Q_INVOKABLE void mainAction();

    Q_INVOKABLE void createBackup(const QUrl &backupUrl);
    Q_INVOKABLE void restoreBackup(const QUrl &backupUrl);
    Q_INVOKABLE void factoryReset();

    Q_INVOKABLE void installFirmware(const QUrl &fileUrl);
    Q_INVOKABLE void installWirelessStack(const QUrl &fileUrl);
    Q_INVOKABLE void installFUS(const QUrl &fileUrl, uint32_t address);

    Q_INVOKABLE void startFullScreenStreaming();
    Q_INVOKABLE void stopFullScreenStreaming();

    Q_INVOKABLE void refreshStorageInfo();
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
    void onDeviceInfoChanged();
    void onDeviceOperationFinished();
    void onDeviceRegistryErrorOccured();
    void onFileManagerErrorOccured();
    void onScreenStreamerStateChanged();

private:
    static void initLibraryPaths();
    static void registerMetaTypes();
#if QT_VERSION < 0x060000
    static void registerComparators();
#endif

    void initConnections();
    void beginUpdate();
    void beginRepair();

    void setBackendState(BackendState newState);
    void setErrorType(BackendError::ErrorType newErrorType);

    Flipper::DeviceRegistry *m_deviceRegistry;
    Flipper::UpdateRegistry *m_firmwareUpdateRegistry;

    Flipper::Zero::ScreenStreamer *m_screenStreamer;
    Flipper::Zero::VirtualDisplay *m_virtualDisplay;
    Flipper::Zero::FileManager *m_fileManager;

    BackendState m_backendState;
    BackendError::ErrorType m_errorType;
};
