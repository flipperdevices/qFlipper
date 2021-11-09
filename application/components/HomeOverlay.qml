import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

Item {
    id: overlay
    visible: opacity > 0
    layer.enabled: true

    property Rectangle backgroundRect

    readonly property int centerX: 590

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 350
        }
    }

    ConfirmationDialog {
        id: confirmationDialog
        parent: backgroundRect
        radius: backgroundRect.radius
    }

//    Dialog {
//        id: confirmationDialog
//        anchors.centerIn: parent

//        title: "Update to version 0.69.4?"

//        contentItem: Text {
//            color: Theme.color.mediumorange1
//            text: "Mate this shit is gonna legit explode I'm proper warning you"
//            horizontalAlignment: Text.AlignHCenter
//            verticalAlignment: Text.AlignVCenter
//        }

//        standardButtons: Dialog.Yes | Dialog.No
//    }

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

        color: Theme.color.orange

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

            color: (!deviceState || !deviceState.isOnline) ? Theme.color.red : deviceState.isRecoveryMode ?
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

        accent: {
            if(!deviceState || !deviceState.isOnline) {
                return accent;
            } else if(deviceState.isRecoveryMode) {
                return UpdateButton.Blue;
            } else {
                // TODO: Check for updates
                return UpdateButton.Green
            }
        }

        x: Math.round(centerX - width / 2)
        y: 265

        onClicked: {
            if(!firmwareUpdates.isReady) {
                return;
            } else if(deviceState.isRecoveryMode) {
                device.updater.fullRepair(firmwareUpdates.latestVersion);
            } else {
                device.updater.fullUpdate(firmwareUpdates.latestVersion);
            }
        }
    }

    LinkButton {
        id: releaseButton
        x: centerX - width - 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 5

        linkColor: {
            if(!firmwareUpdates.isReady) {
                return Theme.color.orange;
            } else if(preferences.updateChannel === "development") {
                return Theme.color.lightred2;
            } else if(preferences.updateChannel === "release-candidate") {
                return "blueviolet";
            } else if(preferences.updateChannel === "release") {
                return Theme.color.lightgreen;
            } else {
                return Theme.color.orange;
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
    }

    LinkButton {
        id: fileButton
        x: centerX + 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 5

        text: "Install from file"
        onClicked: confirmationDialog.open()
    }
}
