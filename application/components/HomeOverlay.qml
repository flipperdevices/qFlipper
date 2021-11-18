import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2

import Theme 1.0

Item {
    id: overlay

    property Rectangle backgroundRect

    readonly property int centerX: 590

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

    visible: opacity > 0

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 350
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
            DeviceActions { id: deviceActions }
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

//            ToolTip.text: qsTr("Device information")
        }

        TabButton {
            icon.source: "qrc:/assets/gfx/symbolic/wrench.svg"
            icon.width: 27
            icon.height: 27

//            ToolTip.text: qsTr("Avdanced controls")
        }
    }

    Text {
        id: nameLabel
        x: centerX - width - 4
        y: 24

        color: Theme.color.lightorange2

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        text: deviceInfo ? deviceInfo.name : text
    }

    ColumnLayout {
        x: centerX + 4
        anchors.bottom: nameLabel.baseline

        TransparentLabel {
            id: connectionLabel
            height: 14

            font.capitalization: Font.AllUppercase

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
        }
    }

    UpdateButton {
        id: updateButton
        action: updateButtonAction

        x: Math.round(centerX - width / 2)
        y: 265

        accent: !(firmwareUpdates.isReady && deviceState) ? accent : deviceState.isRecoveryMode ? UpdateButton.Blue :
                 device.updater.canUpdate(firmwareUpdates.latestVersion) ? UpdateButton.Green : UpdateButton.Default
    }

    LinkButton {
        id: releaseButton
        action: changelogAction
        x: centerX - width - 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 5

        linkColor: {
            if(!firmwareUpdates.isReady) {
                return Theme.color.lightorange2;
            } else if(preferences.updateChannel === "development") {
                return Theme.color.lightred2;
            } else if(preferences.updateChannel === "release-candidate") {
                return "blueviolet";
            } else if(preferences.updateChannel === "release") {
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

        enabled: firmwareUpdates.isReady && !!deviceState &&
                (deviceState.isRecoveryMode || device.updater.canUpdate(firmwareUpdates.latestVersion) || device.updater.canInstall())

        text: !(firmwareUpdates.isReady && deviceState) ? qsTr("No data") : deviceState.isRecoveryMode ? qsTr("Repair") :
               device.updater.canUpdate(firmwareUpdates.latestVersion) ? qsTr("Update") :
               device.updater.canInstall() ? qsTr("Install") : qsTr("No updates")

        onTriggered: updateButtonFunc()
    }

    Action {
        id: changelogAction
        enabled: firmwareUpdates.isReady

        text: {
            let str;

            if(!firmwareUpdates.isReady) {
                return qsTr("No data");
            } else if(preferences.updateChannel === "development") {
                str = "Dev";
            } else if(preferences.updateChannel === "release-candidate") {
                str = "RC";
            } else if(preferences.updateChannel === "release") {
                str = "Release";
            } else {
                str = "Unknown";
            }

            return "%1 %2".arg(str).arg(firmwareUpdates.latestVersion.number);
        }

        onTriggered: changelogDialog.open()
    }

    Action {
       id: installFromFileAction
       text: qsTr("Install from file")
       onTriggered: installFromFile()
    }

    function updateButtonFunc() {
        const channelName = preferences.updateChannel;
        const latestVersion = firmwareUpdates.latestVersion;

        let messageObj, actionFunc;

        if(deviceState.isRecoveryMode) {
            messageObj = {
                title : qsTr("Repair %1?").arg(deviceInfo.name),
                message: "%1 %2".arg(channelName).arg(latestVersion.number),
                description: qsTr("User settings will be erased.")
            };

            actionFunc = function() {
                device.updater.fullRepair(latestVersion);
            }

        } else {
            messageObj = {
                title : qsTr("Update %1?").arg(deviceInfo.name),
                message: "Version %1".arg(latestVersion.number),
                description : qsTr("This will install the latest <font color=\"%1\">%2</font> version.")
                              .arg(releaseButton.linkColor).arg(channelName.toUpperCase())
            };

            actionFunc = function() {
                device.updater.fullUpdate(latestVersion);
            }
        }

        confirmationDialog.openWithMessage(actionFunc, messageObj);
    }

    function installFromFile() {
        fileDialog.selectFolder = false;
        fileDialog.title = qsTr("Please choose a firmware file");
        fileDialog.nameFilters = ["Firmware files (*.dfu)", "All files (*.*)"];
        fileDialog.onAcceptedFunc = function() {
            const messageObj = {
                title : qsTr("Flash %1?").arg(deviceInfo.name),
                message: baseName(fileDialog.fileUrl),
                description : qsTr("Installing firmware from a file is <font color=\"%1\">not</font> recommended.")
                              .arg(Theme.color.lightred3)
            };

            const actionFunc = function() {
                device.updater.localFirmwareInstall(fileDialog.fileUrl);
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
                title : qsTr("Backup %1?").arg(deviceInfo.name),
                message: qsTr("Internal storage backup"),
                description : qsTr("This will backup your device settings.")
            };

            const actionFunc = function() {
                device.updater.backupInternalStorage(fileDialog.fileUrl);
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
                title : qsTr("Restore %1?").arg(deviceInfo.name),
                message: qsTr("Internal storage restore"),
                description : qsTr("This will restore your device settings.")
            };

            const actionFunc = function() {
                device.updater.restoreInternalStorage(fileDialog.fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        }

        fileDialog.open();
    }

    function eraseDevice() {
        const messageObj = {
            title : qsTr("Erase %1?").arg(deviceInfo.name),
            message: qsTr("Erase device settings"),
            description : "<font color=\"%1\">%2</font>"
                          .arg(Theme.color.lightred3)
                          .arg(qsTr("Warning! This will reset your device settings to factory defaults!"))
        };

        const actionFunc = function() {
            device.updater.factoryReset();
        }

        confirmationDialog.openWithMessage(actionFunc, messageObj);
    }

    function reinstallFirmware() {
        const messageObj = {
            title : qsTr("Reinstall %1?").arg(deviceInfo.name),
            message: qsTr("Reinstall current firmware"),
            description : qsTr("This will install the currently installed version again.")
        };

        const actionFunc = function() {
            device.updater.fullUpdate(firmwareUpdates.latestVersion);
        }

        confirmationDialog.openWithMessage(actionFunc, messageObj);
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
    }
}
