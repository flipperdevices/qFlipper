import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    signal updateRequested(flipperInfo: var)
    signal localUpdateRequested(flipperInfo: var)
    signal versionListRequested(flipperInfo: var)

    id: item
    width: parent.width
    height: 85

    StyledProgressBar {
        id: progressBar
        anchors.fill: parent
        anchors.margins: frame.border.width
//        value: progress
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
        text: display.model
        color: "darkgray"
        font.pointSize: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 15
    }

    Rectangle {
        id: nameLabel
        color: display.isDFU ? "#0345ff" : "darkorange"
        width: 100
        height: 30

        radius: height/2

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: modelLabel.right
        anchors.leftMargin: 10

        Text {
            text: display.name
            color: "black"
            font.pointSize: 12
            font.bold: true
            anchors.centerIn: parent
        }
    }

    StyledButton {
        id: updateButton
//        text: message
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.verticalCenter: parent.verticalCenter
        enabled: text === qsTr("Update")
        suggested: !display.isDFU
    }

    Text {
        id: versionLabel
        text: qsTr("version ") + display.version
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
//            onTriggered: versionListRequested(info)
        }

        MenuItem {
            text: qsTr("Update from local file...")
//            onTriggered: localUpdateRequested(info)
        }
    }

//    Connections {
//        target: updateButton
//        function onClicked() { updateRequested(info) }
//        function onPressAndHold() { actionMenu.open(); }
//    }
}
