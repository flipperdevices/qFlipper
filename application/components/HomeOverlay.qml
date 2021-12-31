import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import QtQml.Models 2.15

import Theme 1.0
import QFlipper 1.0

AbstractOverlay {
    id: overlay

    signal selfUpdateRequested
    readonly property int centerX: 590
    readonly property int centerOffset: Math.min(overlay.width - (centerX + systemPathLabel.width + 12), 0)

    TabButton {
        id: dangerTab
        icon.source: "qrc:/assets/gfx/symbolic/skull.svg"
        icon.width: 27
        icon.height: 27

        enabled: App.dangerousFeatures
        visible: App.dangerousFeatures

        ToolTip {
            text: qsTr("Wanna have a bad time?")
            visible: parent.hovered
        }
    }

    FileDialog {
        id: fileDialog
        folder: shortcuts.home
        selectExisting: true
        selectMultiple: false

        property var onAcceptedFunc
        onAccepted: onAcceptedFunc()
    }

    ConfirmationDialog {
        id: confirmationDialog
        parent: backgroundRect
        radius: backgroundRect.radius
    }

    ChangelogDialog {
        id: changelogDialog
        parent: backgroundRect
        radius: backgroundRect.radius
    }

    TabPane {
        width: 322

        anchors.top: tabs.bottom
        anchors.left: tabs.left
        anchors.topMargin: -2

        currentIndex: tabs.currentIndex

        items: [
            DeviceInfo { id: deviceInfoPane },
            DeviceActions { id: deviceActions },
            AdvancedDeviceActions { id: advDeviceActions }
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
    }

    TextLabel {
        id: nameLabel
        x: centerX + centerOffset - width - 4
        y: 24

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
        }
    }

    MainButton {
        id: updateButton
        action: updateButtonAction

        x: Math.round(centerX - width / 2)
        y: 265

        accent: {
            switch(Backend.updateStatus) {
            case Backend.CanRepair:
                return MainButton.Blue;
            case Backend.CanUpdate:
                return MainButton.Green;
            default:
                return MainButton.Default;
            }
        }
    }

    LinkButton {
        id: releaseButton
        action: changelogAction
        x: centerX - width - 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 5

        linkColor: {
            if(Backend.UpdateStatus === Backend.Unknown) {
                return Theme.color.lightorange2;
            } else if(Preferences.updateChannel === "development") {
                return Theme.color.lightred2;
            } else if(Preferences.updateChannel === "release-candidate") {
                return "blueviolet";
            } else if(Preferences.updateChannel === "release") {
                return Theme.color.lightgreen;
            } else {
                return Theme.color.lightorange2;
            }
        }
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

        enabled: (Backend.updateStatus !== Backend.Unknown) &&
                 (Backend.updateStatus !== Backend.NoUpdates)
        text: {
            switch(Backend.updateStatus) {
            case Backend.CanRepair:
                return qsTr("Repair");
            case Backend.CanUpdate:
                return qsTr("Update");
            case Backend.CanInstall:
                return qsTr("Install");
            case Backend.NoUpdates:
                return qsTr("No updates");
            case Backend.Unknown:
                return qsTr("No data");
            }
        }

        onTriggered: updateButtonFunc()
    }

    Action {
        id: changelogAction
        enabled: Backend.updateStatus !== Backend.Unknown

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

            return "%1 %2".arg(str).arg(firmwareUpdates.latestVersion.number.split("-")[0]);
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
        const latestVersion = firmwareUpdates.latestVersion;

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

        confirmationDialog.openWithMessage(Backend.mainAction, messageObj);
    }

    function installFromFile() {
        fileDialog.selectFolder = false;
        fileDialog.title = qsTr("Please choose a firmware file");
        fileDialog.nameFilters = ["Firmware files (*.dfu)", "All files (*.*)"];
        fileDialog.onAcceptedFunc = function() {
            const messageObj = {
                title : qsTr("Install from file?"),
                customText: qsTr("Install"),
                message: qsTr("Firmware from file %1<br/>will be installed").arg(baseName(fileDialog.fileUrl))
            };

            const actionFunc = function() {
                Backend.installFirmware(fileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        };

        fileDialog.open();
    }

    function backupDevice() {
        fileDialog.selectFolder = true;
        fileDialog.title = qsTr("Please choose backup directory");

        fileDialog.onAcceptedFunc = function() {
            const messageObj = {
                title : qsTr("Backup device?"),
                customText: qsTr("Backup"),
                message: qsTr("Device settings will be backed up")
            };

            const actionFunc = function() {
                Backend.createBackup(fileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        }

        fileDialog.open();
    }

    function restoreDevice() {
        fileDialog.selectFolder = true;
        fileDialog.title = qsTr("Please choose backup directory");

        fileDialog.onAcceptedFunc = function() {
            const messageObj = {
                title : qsTr("Restore backup?"),
                customText: qsTr("Restore"),
                message: qsTr("Device settings will be restored<br/>from selected backup")
            };

            const actionFunc = function() {
                Backend.restoreBackup(fileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        }

        fileDialog.open();
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

        confirmationDialog.openWithMessage(Backend.mainAction, messageObj);
    }

    function installWirelessStack() {
        fileDialog.selectFolder = false;
        fileDialog.title = qsTr("Please choose a firmware file");
        fileDialog.nameFilters = ["Firmware files (*.bin)", "All files (*.*)"];
        fileDialog.onAcceptedFunc = function() {
            const messageObj = {
                title : qsTr("Install wireless stack?"),
                customText: qsTr("Install"),
                suggestedRole: ConfirmationDialog.RejectRole,
                message: qsTr("WARNING! This operaton can break your Flipper!")
            };

            const actionFunc = function() {
                Backend.installWirelessStack(fileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        };

        fileDialog.open();
    }

    function installFUSDangerDanger() {
        fileDialog.selectFolder = false;
        fileDialog.title = qsTr("Please choose a firmware file");
        fileDialog.nameFilters = ["Firmware files (*.bin)", "All files (*.*)"];
        fileDialog.onAcceptedFunc = function() {
            const messageObj = {
                title : qsTr("Install FUS?"),
                customText: qsTr("Install"),
                suggestedRole: ConfirmationDialog.RejectRole,
                message: qsTr("LAST WARNING! This will invalidate your encryption keys! Please reconsider.")
            };

            const actionFunc = function() {
                Backend.installFUS(fileDialog.fileUrl, 0x080ec00);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        };

        fileDialog.open();
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
        advDeviceActions.installRadioAction.triggered.connect(installWirelessStack);
        advDeviceActions.installFusAction.triggered.connect(installFUSDangerDanger);

        if(App.dangerousFeatures) {
            tabs.addItem(dangerTab);
        }
    }
}
