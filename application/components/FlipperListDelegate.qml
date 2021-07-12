import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    signal updateRequested(var device)
    signal localUpdateRequested(var device)
    signal localRadioUpdateRequested(var device)
    signal localFUSUpdateRequested(var device)

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
        id: menuButton
        width: height

        icon.source: "qrc:/assets/menu.svg"
        display: AbstractButton.IconOnly

        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.verticalCenter: parent.verticalCenter

        enabled: updateButton.enabled

        onClicked: actionMenu.open()
    }

    StyledButton {
        id: updateButton
        text: device.statusMessage

        anchors.right: menuButton.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter

        enabled: text === qsTr("Update")
        dangerous: text === qsTr("Error")

        onClicked: updateRequested(device)
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
        x: menuButton.x + menuButton.width - width
        y: menuButton.y + menuButton.height + 4

        MenuItem {
            text: qsTr("Other versions...")
            onTriggered: versionListRequested(device)
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

        MenuItem {
            text: qsTr("Update FUS (Expert)...")
            onTriggered: localFUSUpdateRequested(device)
        }

        MenuItem {
            text: qsTr("Update Radio (Expert)...")
            onTriggered: localRadioUpdateRequested(device)
        }
    }

    Component.onCompleted: {
        updateRegistry.latestVersionChanged.connect(function() {
            updateButton.suggested = (!device.isDFU) && (updateRegistry.latestVersion(device.target) > device.version);
        })
    }
}
