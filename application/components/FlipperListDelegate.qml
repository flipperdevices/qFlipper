import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    signal updateRequested(var device)

    signal versionListRequested(var device)
    signal screenStreamRequested(var device)

    signal localUpdateRequested(var device)
    signal localRadioUpdateRequested(var device)
    signal localFUSUpdateRequested(var device)

    signal fixBootRequested(var device)

    id: item
    width: parent.width
    height: 85

    StyledProgressBar {
        id: progressBar
        anchors.fill: parent
        anchors.margins: frame.border.width
        value: device.state.progress
    }

    Rectangle {
        id: frame
        radius: 6
        anchors.fill: parent
        color: device.state.isError ? "#3a0000" : "transparent"
        border.color: device.state.isError ? "#d32a34" : "white"
        border.width: 1
    }

    Text {
        id: modelLabel
        text: device.state.model
        color: "darkgray"
        font.pixelSize: 13
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 15
    }

    Rectangle {
        id: nameLabel
        color: device.state.isError ? "#d32a34" : (device.state.isRecoveryMode ? "#0345ff" : "darkorange")
        width: 100
        height: 30

        radius: height/2

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: modelLabel.right
        anchors.leftMargin: 10

        Text {
            text: device.state.name
            color: "black"
            font.pixelSize: 16
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

        enabled: !device.state.isPersistent && !device.state.isError

        onClicked: actionMenu.open()
    }

    StyledButton {
        id: updateButton
        text: {
            if(!firmwareUpdates.isReady) {
                return qsTr("Error");
            } else if(device.state.isRecoveryMode) {
                return qsTr("Repair");
            } else if(device.updater.canChangeChannel(firmwareUpdates.latestVersion)) {
                return qsTr("Change");
            } else if(device.updater.canUpdate(firmwareUpdates.latestVersion)) {
                return qsTr("Update");
            } else if(device.updater.canRollback(firmwareUpdates.latestVersion)) {
                return qsTr("Rollback");
            } else {
                return qsTr("Reinstall");
            }
        }

        suggested: device.state.isRecoveryMode ? false : device.updater.canUpdate(firmwareUpdates.latestVersion)
        visible: firmwareUpdates.isReady && !(device.state.isPersistent || device.state.isError)

        anchors.right: menuButton.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter

        onClicked: updateRequested(device)
    }

    Text {
        id: versionLabel
        visible: !(messageLabel.visible || device.state.isRecoveryMode)
        text: qsTr("version ") + device.state.version
        font.pixelSize: 13

        anchors.left: nameLabel.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10

        color: "darkgray"
    }

    Text {
        id: messageLabel
        text: device.state.isError ? device.state.errorString : device.state.statusString
        visible: device.state.isPersistent || device.state.isError
        color: device.state.isError ? "#ddd" : "white"

        font.pixelSize: 13

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
            enabled: firmwareUpdates.isReady
        }


        MenuSeparator {}

        MenuItem {
            text: qsTr("Screen Streaming...")
            onTriggered: screenStreamRequested(device)
            enabled: !device.state.isRecoveryMode
        }

        MenuSeparator {}

        Menu {
            title: qsTr("Expert options")

        MenuItem {
            text: qsTr("Update Main firmware...")
//            onTriggered: localUpdateRequested(device)
        }
            MenuItem {
                text: qsTr("Update Radio firmware...")
                onTriggered: localRadioUpdateRequested(device)
            }

            MenuItem {
                text: qsTr("Update FUS (not recommended)...")
//                onTriggered: localFUSUpdateRequested(device)
            }
        }
    }
}
