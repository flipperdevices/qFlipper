import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15

import Theme 1.0
import QFlipper 1.0

AbstractOverlay {
    id: overlay

    signal selfUpdateRequested
    readonly property int centerX: 590
    readonly property int centerOffset: Math.min(overlay.width - (centerX + systemPathLabel.width + 12), 0)

    onDeviceInfoChanged: tabs.currentIndex = 0;

    TabButton {
        id: developerTab
        icon.source: "qrc:/assets/gfx/symbolic/developer-mode.svg"
        icon.width: 27
        icon.height: 27

        enabled: App.isDeveloperMode
        visible: App.isDeveloperMode

        ToolTip {
            text: qsTr("Developer mode. Use with caution!")
            visible: parent.hovered
        }
    }

    MessageDialog {
        id: messageDialog
        parent: backgroundRect
        radius: backgroundRect.radius
    }

    ConfirmationDialog {
        id: confirmationDialog
        parent: backgroundRect
        radius: backgroundRect.radius
    }

    ProgressDialog {
        id: progressDialog
        parent: backgroundRect
        radius: backgroundRect.radius

        title: qsTr("Please wait")
        text: qsTr("File operation in progress...")

        value: deviceState ? deviceState.progress : -1
        indeterminate: deviceState ? deviceState.progress < 0 : true

        Component.onCompleted: {
            Backend.fileManager.isBusyChanged.connect(function() {
                if(Backend.fileManager.isBusy) open();
                else close();
            });
        }
    }

    ChangelogDialog {
        id: changelogDialog
        parent: backgroundRect
        radius: backgroundRect.radius
    }

    SDWarningDialog {
        id: sdWarningDialog
        parent: backgroundRect
        radius: backgroundRect.radius
        onRejected: Backend.refreshStorageInfo()
        onAccepted: {
            updateButtonFunc();
            result = false;
        }
    }

    TabPane {
        z: fileButton.z + 1
        anchors.top: tabs.bottom
        anchors.left: tabs.left
        anchors.topMargin: -2

        currentIndex: tabs.currentIndex
        backgroundColor: Qt.rgba(0, 0, 0, fileManagerTab.checked)

        items: [
            DeviceInfo { id: deviceInfoPane },
            DeviceActions { id: deviceActions },
            FileManager { id: fileManager; messageDialog: messageDialog; confirmationDialog: confirmationDialog; },
            DeveloperActions { id: developerActions }
        ]
    }

    TabBar {
        id: tabs
        x: 28
        y: 28

        layer.enabled: true

        TabButton {
            icon.source: "qrc:/assets/gfx/symbolic/info.svg"
            icon.width: 25
            icon.height: 25

            ToolTip {
                text: qsTr("Device information")
                visible: parent.hovered
            }
        }

        TabButton {
            icon.source: "qrc:/assets/gfx/symbolic/wrench.svg"
            icon.width: 27
            icon.height: 27

            ToolTip {
                text: qsTr("Advanced controls")
                visible: parent.hovered
            }
        }

        TabButton {
            id: fileManagerTab
            enabled: Backend.deviceState && !Backend.deviceState.isRecoveryMode

            icon.source: "qrc:/assets/gfx/symbolic/file-symbolic.svg"
            icon.width: 23
            icon.height: 29

            onCheckedChanged: if(checked) Backend.fileManager.refresh()

            ToolTip {
                text: qsTr("File manager")
                visible: parent.hovered
            }
        }
    }

    TextLabel {
        id: nameLabel
        x: centerX + centerOffset - width - 4
        y: 19

        color: Theme.color.lightorange2

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        capitalized: false
        text: deviceInfo ? deviceInfo.name : text
    }

    ColumnLayout {
        x: centerX + centerOffset + 4
        anchors.bottom: nameLabel.baseline

        TransparentLabel {
            id: connectionLabel
            height: 14

            icon.source: "qrc:/assets/gfx/symbolic/usb-connected.svg"
            icon.width: 18
            icon.height: 10

            color: (!deviceState || !deviceState.isOnline) ? Theme.color.lightred1 : deviceState.isRecoveryMode ?
                                           Theme.color.lightblue : Theme.color.lightgreen
            text: (!deviceState || !deviceState.isOnline) ? qsTr("Disconnected") : deviceState.isRecoveryMode ?
                                           qsTr("Recovery mode") : qsTr("Connected")
        }

        TransparentLabel {
            id: systemPathLabel
            height: connectionLabel.height
            color: connectionLabel.color
            text: deviceInfo ? deviceInfo.systemLocation : text
            capitalized: false

            TextEdit {
                id: systemPathLabelTextCopy
                text: parent.text
                visible: false
            }

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    systemPathLabelTextCopy.selectAll();
                    systemPathLabelTextCopy.copy();
                }
            }
        }
    }

    MainButton {
        id: updateButton
        action: updateButtonAction

        x: Math.round(centerX - width / 2)
        y: 265

        accent: {
            switch(Backend.firmwareUpdateState) {
            case ApplicationBackend.CanRepair:
                return MainButton.Blue;
            case ApplicationBackend.CanUpdate:
                return MainButton.Green;
            default:
                return MainButton.Default;
            }
        }

        icon.source: Backend.firmwareUpdateState === ApplicationBackend.ErrorOccured ? "qrc:/assets/gfx/symbolic/update-symbolic.svg" : ""
        icon.width: 32
        icon.height: 32

        ToolTip {
            text: {
                switch(Backend.firmwareUpdateState) {
                case ApplicationBackend.CanRepair:
                    return qsTr("Repair a broken firmware installation. May erase your progress and settings.");
                case ApplicationBackend.CanUpdate:
                    return qsTr("Update Flipper to the latest version");
                case ApplicationBackend.CanInstall:
                    return qsTr("Install firmware from currently selected update channel");
                case ApplicationBackend.ErrorOccured:
                    return qsTr("Press to check internet connection and try to update Flipper again");
                default:
                    return "";
                }
            }

            implicitWidth: 300
            visible: parent.hovered && text.length !== 0
        }
    }

    LinkButton {
        id: releaseButton
        action: changelogAction
        x: centerX - width - 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 5

        linkColor: {
            if(Preferences.updateChannel === "development") {
                return Theme.color.lightred2;
            } else if(Preferences.updateChannel === "release-candidate") {
                return "blueviolet";
            } else if(Preferences.updateChannel === "release") {
                return Theme.color.lightgreen;
            } else {
                return Theme.color.lightorange2;
            }
        }

        visible: Backend.firmwareUpdateState !== ApplicationBackend.Unknown &&
                 Backend.firmwareUpdateState !== ApplicationBackend.Checking &&
                 Backend.firmwareUpdateState !== ApplicationBackend.ErrorOccured
    }

    LinkButton {
        id: fileButton
        x: centerX + 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 5

        action: installFromFileAction
    }

    Action {
        id: updateButtonAction

        enabled: Backend.firmwareUpdateState === ApplicationBackend.CanUpdate ||
                 Backend.firmwareUpdateState === ApplicationBackend.CanInstall ||
                 Backend.firmwareUpdateState === ApplicationBackend.CanRepair ||
                 Backend.firmwareUpdateState === ApplicationBackend.ErrorOccured

        text: {
            switch(Backend.firmwareUpdateState) {
            case Backend.Unknown:
                return qsTr("No data");
            case Backend.Checking:
                return qsTr("Checking...");
            case Backend.CanRepair:
                return qsTr("Repair");
            case Backend.CanUpdate:
                return qsTr("Update");
            case Backend.CanInstall:
                return qsTr("Install");
            case Backend.NoUpdates:
                return qsTr("No updates");
            case Backend.ErrorOccured:
                return qsTr("Try again");
            }
        }

        onTriggered: Backend.firmwareUpdateState !== ApplicationBackend.ErrorOccured ?
                     updateButtonFunc() : Backend.checkFirmwareUpdates()
    }

    Action {
        id: changelogAction
        enabled: Backend.firmwareUpdateState !== ApplicationBackend.Unknown &&
                 Backend.firmwareUpdateState !== ApplicationBackend.Checking &&
                 Backend.firmwareUpdateState !== ApplicationBackend.ErrorOccured

        text: {
            let str;

            if(!enabled) {
                return qsTr("No data");
            } else if(Preferences.updateChannel === "development") {
                str = "Dev";
            } else if(Preferences.updateChannel === "release-candidate") {
                str = "RC";
            } else if(Preferences.updateChannel === "release") {
                str = "Release";
            } else {
                str = "Unknown";
            }

            return "%1 %2".arg(str).arg(Backend.latestFirmwareVersion.number.split("-")[0]);
        }

        onTriggered: changelogDialog.open()
    }

    Action {
       id: installFromFileAction
       text: qsTr("Install from file")
       onTriggered: installFromFile()
    }

    function updateButtonFunc() {
        const channelName = Preferences.updateChannel;
        const messageObj = deviceState.isRecoveryMode ? {
                title : qsTr("Repair Device?"),
                customText: qsTr("Repair"),
                message : qsTr("Firmware <font color=\"%1\">%2</font><br/>will be installed")
                          .arg(releaseButton.linkColor)
                          .arg(releaseButton.text)
            } : {
                title : qsTr("Update firmware?"),
                customText: qsTr("Update"),
                message: qsTr("New firmware <font color=\"%1\">%2</font><br/>will be installed")
                         .arg(releaseButton.linkColor)
                         .arg(releaseButton.text),
            };

        const canUpdate = deviceInfo.storage.isExternalPresent ||
                          deviceState.isRecoveryMode ||
                          sdWarningDialog.result;
        if(canUpdate) {
            confirmationDialog.openWithMessage(Backend.mainAction, messageObj);
        } else {
            sdWarningDialog.open();
        }
    }

    function installFromFile() {
        SystemFileDialog.accepted.connect(function() {
            const messageObj = {
                title : qsTr("Install from file?"),
                customText: qsTr("Install"),
                message: qsTr("Firmware from file %1<br/>will be installed").arg(baseName(SystemFileDialog.fileUrl))
            };

            const actionFunc = function() {
                Backend.installFirmware(SystemFileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        });

        const nameFilters = (deviceState.isRecoveryMode ? [] : ["Firmware bundle files (*.tgz)"]).concat(["Firmware files (*.dfu)", "All files (*.*)"]);
        SystemFileDialog.beginOpenFile(SystemFileDialog.LastLocation, nameFilters);
    }

    function backupDevice() {
        SystemFileDialog.accepted.connect(function() {
            const messageObj = {
                title : qsTr("Backup device?"),
                customText: qsTr("Backup"),
                message: qsTr("Device settings will be backed up")
            };

            const actionFunc = function() {
                Backend.createBackup(SystemFileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        });

        const defaultName = "%1-backup-%2.tgz".arg(deviceInfo.name).arg(Qt.formatDateTime(new Date(), "yyyyMMdd-hhmmss"));
        SystemFileDialog.beginSaveFile(SystemFileDialog.LastLocation, ["Backup files (*.tgz)", "All files (*.*)"], defaultName);
    }

    function restoreDevice() {
        SystemFileDialog.accepted.connect(function() {
            const messageObj = {
                title : qsTr("Restore backup?"),
                customText: qsTr("Restore"),
                message: qsTr("Device settings will be restored<br/>from selected backup")
            };

            const actionFunc = function() {
                Backend.restoreBackup(SystemFileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        });

        SystemFileDialog.beginOpenFile(SystemFileDialog.LastLocation, ["Backup files (*.tgz)", "All files (*.*)"]);
    }

    function eraseDevice() {
        const messageObj = {
            title : qsTr("Erase device?"),
            message: qsTr("Device settings will be fully erased"),
            suggestedRole: ConfirmationDialog.RejectRole,
            customText: qsTr("Erase")
        };

        confirmationDialog.openWithMessage(Backend.factoryReset, messageObj);
    }

    function reinstallFirmware() {
        const messageObj = {
            title : qsTr("Reinstall firmware?"),
            customText: qsTr("Reinstall"),
            message: qsTr("Current firmware version will be reinstalled")
        };

        const canReinstall = deviceInfo.storage.isExternalPresent ||
                             deviceState.isRecoveryMode ||
                             sdWarningDialog.result;
        if(canReinstall) {
            confirmationDialog.openWithMessage(Backend.mainAction, messageObj);
        } else {
            sdWarningDialog.open();
        }
    }

    function installWirelessStack() {
        SystemFileDialog.accepted.connect(function() {
            const messageObj = {
                title : qsTr("Install wireless stack?"),
                customText: qsTr("Install"),
                suggestedRole: ConfirmationDialog.RejectRole,
                message: qsTr("WARNING! This operaton can break your Flipper!")
            };

            const actionFunc = function() {
                Backend.installWirelessStack(SystemFileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        });

        SystemFileDialog.beginOpenFile(SystemFileDialog.LastLocation, ["Firmware files (*.bin)", "All files (*.*)"]);
    }

    function installFUSDangerDanger() {
        SystemFileDialog.accepted.connect(function() {
            const messageObj = {
                title : qsTr("Install FUS?"),
                customText: qsTr("Install"),
                suggestedRole: ConfirmationDialog.RejectRole,
                message: qsTr("LAST WARNING! This will invalidate your encryption keys! Please reconsider.")
            };

            const actionFunc = function() {
                Backend.installFUS(SystemFileDialog.fileUrl, 0x080ec00);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        });

        SystemFileDialog.beginOpenFile(SystemFileDialog.LastLocation, ["Firmware files (*.bin)", "All files (*.*)"]);
    }

    function baseName(fileUrl) {
        const str = fileUrl.toString();
        return str.slice(str.lastIndexOf("/") + 1);
    }

    Component.onCompleted: {
        deviceActions.backupAction.triggered.connect(backupDevice);
        deviceActions.restoreAction.triggered.connect(restoreDevice);
        deviceActions.eraseAction.triggered.connect(eraseDevice);
        deviceActions.reinstallAction.triggered.connect(reinstallFirmware);
        deviceActions.selfUpdateAction.triggered.connect(selfUpdateRequested);
        developerActions.installRadioAction.triggered.connect(installWirelessStack);
        developerActions.installFusAction.triggered.connect(installFUSDangerDanger);

        if(App.isDeveloperMode) {
            tabs.addItem(developerTab);
        }

        // Close dialog windows when Flipper was PIN locked/disconnected
        Backend.currentDeviceChanged.connect(function() {
            if(messageDialog.visible) {
                messageDialog.close();
            }

            if(confirmationDialog.visible) {
                confirmationDialog.close();
            }

            if(sdWarningDialog.visible) {
                sdWarningDialog.close();
            }
        });
    }
}
