import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    signal updateRequested(var device)
    signal localUpdateRequested(var device)
    signal localRadioUpdateRequested(var device)
    signal localFUSUpdateRequested(var device)

    signal fixOptionBytesRequested(var device)
    signal fixBootRequested(var device)

    signal versionListRequested(var device)
    signal screenStreamRequested(var device)

    id: item
    width: parent.width
    height: 85

    StyledProgressBar {
        id: progressBar
        anchors.fill: parent
        anchors.margins: frame.border.width
        value: device.progress
    }

    Rectangle {
        id: frame
        radius: 6
        anchors.fill: parent
        color: device.isError ? "#3a0000" : "transparent"
        border.color: device.isError ? "#d32a34" : "white"
        border.width: 1
    }

    Text {
        id: modelLabel
        text: device.model
        color: "darkgray"
        font.pointSize: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 15
    }

    Rectangle {
        id: nameLabel
        color: device.isError ? "#d32a34" : (device.isDFU ? "#0345ff" : "darkorange")
        width: 100
        height: 30

        radius: height/2

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: modelLabel.right
        anchors.leftMargin: 10

        Text {
            text: device.name
            color: "black"
            font.pointSize: 12
            font.bold: true
            anchors.centerIn: parent
        }
    }

    StyledButton {
        id: menuButton
        width: height

        icon.source: "qrc:/assets/menu.svg"
        display: AbstractButton.IconOnly

        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.verticalCenter: parent.verticalCenter

        enabled: !device.isPersistent && !device.isError

        onClicked: actionMenu.open()
    }

    StyledButton {
        id: updateButton
        text: {
            if(updateRegistry.channelNames.length === 0) {
                return qsTr("Error");
            } else if(device.isDFU || (device.version === "N/A")) {
                return qsTr("Repair");
            }

            const channelName = "release";
            const latestVersion = updateRegistry.channel(channelName).latestVersion;

            if(latestVersion.number > device.version) {
                return qsTr("Update");
            } else if(latestVersion.number < device.version) {
                return qsTr("Downgrade");
            } else {
                return qsTr("Reinstall");
            }
        }

        suggested: {
            if(device.isDFU || (updateRegistry.channelNames.length === 0)) {
                return false;
            }

            const channelName = "release";
            const latestVersion = updateRegistry.channel(channelName).latestVersion;

            return latestVersion.number > device.version;
        }

        visible: (updateRegistry.channelNames.length > 0) && !(device.isPersistent || device.isError)

        anchors.right: menuButton.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter

        onClicked: updateRequested(device)
    }

    Text {
        id: versionLabel
        visible: !(messageLabel.visible || device.isDFU)
        text: qsTr("version ") + device.version
        font.pointSize: 10

        anchors.left: nameLabel.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10

        color: "darkgray"
    }

    Text {
        id: messageLabel
        text: device.statusMessage
        visible: device.isPersistent || device.isError
        color: device.isError ? "#ddd" : "white"

        font.pointSize: 10

        anchors.left: nameLabel.right
        anchors.right: menuButton.left
        anchors.verticalCenter: parent.verticalCenter

        anchors.leftMargin: 16
        anchors.rightMargin: 16

        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap

        linkColor: "darkorange"
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Menu {
        id: actionMenu
        x: menuButton.x + menuButton.width - width
        y: menuButton.y + menuButton.height + 4

        MenuItem {
            text: qsTr("Other versions...")
            onTriggered: versionListRequested(device)
            enabled: updateRegistry.channelNames.length > 0
        }

        MenuItem {
            text: qsTr("Update from local file...")
            onTriggered: localUpdateRequested(device)
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Screen Streaming...")
            onTriggered: screenStreamRequested(device)
            enabled: !device.isDFU
        }

        MenuSeparator {}

        Menu {
            title: qsTr("Expert options")

            MenuItem {
                text: qsTr("Update Wireless stack...")
                onTriggered: localRadioUpdateRequested(device)
            }

            MenuItem {
                text: qsTr("Update FUS...")
                onTriggered: localFUSUpdateRequested(device)
            }

            MenuItem {
                text: qsTr("Fix Option Bytes...")
                onTriggered: fixOptionBytesRequested(device)
            }

            MenuItem {
                text: qsTr("Fix boot issues")
                onTriggered: fixBootRequested(device)
                enabled: device.isDFU
            }
        }
    }
}
