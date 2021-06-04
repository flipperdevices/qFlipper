import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    signal updateRequested(var device)
    signal localUpdateRequested(var device)
    signal versionListRequested(var device)

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
        color: "transparent"
        border.color: "white"
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
        color: device.isDFU ? "#0345ff" : "darkorange"
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
        id: updateButton
        text: device.statusMessage

        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.verticalCenter: parent.verticalCenter

        enabled: text === qsTr("Update")
        dangerous: text === qsTr("Error")
        suggested: (!device.isDFU) && (updateRegistry.latestVersion(device.target) > device.version)

        onClicked: updateRequested(device)
        onPressAndHold: actionMenu.open()
    }

    Text {
        id: versionLabel
        text: qsTr("version ") + device.version
        font.pointSize: 10

        anchors.left: nameLabel.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10

        color: "darkgray"
    }

    Menu {
        id: actionMenu
        x: updateButton.x
        y: updateButton.y + updateButton.height + 4

        MenuItem {
            text: qsTr("Other versions...")
            onTriggered: versionListRequested(device)
        }

        MenuItem {
            text: qsTr("Update from local file...")
            onTriggered: localUpdateRequested(device)
        }
    }
}
