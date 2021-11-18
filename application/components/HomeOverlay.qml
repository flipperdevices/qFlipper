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

    function baseName(fileUrl) {
        const str = fileUrl.toString();
        return str.slice(str.lastIndexOf("/") + 1);
    }

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 350
        }
    }

    FileDialog {
        id: fileDialog

        title: qsTr("Please choose a firmware file")
        folder: shortcuts.home
        selectMultiple: false
        selectExisting: true

        nameFilters: ["Firmware files (*.dfu)", "All files (*.*)"]

        onAccepted: {
            const messageObj = {
                title : qsTr("Flash %1?").arg(deviceInfo.name),
                message: baseName(fileUrl),
                description : qsTr("Installing firmware from a file is <font color=\"%1\">not</font> recommended.")
                              .arg(Theme.color.lightred3)
            };

            const actionFunc = function() {
                device.updater.localFirmwareUpdate(fileUrl);
            }

            confirmationDialog.openWithMessage(actionFunc, messageObj);
        }
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
            DeviceInfo {},
            DeviceActions {}
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

        x: Math.round(centerX - width / 2)
        y: 265

        enabled: firmwareUpdates.isReady && !!deviceState &&
                (deviceState.isRecoveryMode || device.updater.canUpdate(firmwareUpdates.latestVersion) || device.updater.canInstall())

        text: !(firmwareUpdates.isReady && deviceState) ? qsTr("No data") : deviceState.isRecoveryMode ? qsTr("Repair") :
               device.updater.canUpdate(firmwareUpdates.latestVersion) ? qsTr("Update") :
               device.updater.canInstall() ? qsTr("Install") : qsTr("No updates")

        accent: !(firmwareUpdates.isReady && deviceState) ? accent : deviceState.isRecoveryMode ? UpdateButton.Blue :
                 device.updater.canUpdate(firmwareUpdates.latestVersion) ? UpdateButton.Green : UpdateButton.Default

        onClicked: {
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
    }

    LinkButton {
        id: releaseButton
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

        text: {
            let str;

            if(!firmwareUpdates.isReady) {
                return qsTr("No update data");
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

        onClicked: {
            changelogDialog.open()
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
       id: installFromFileAction
       text: qsTr("Install from file")

       onTriggered: {
            fileDialog.open();
       }
    }
}
