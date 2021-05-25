import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    signal updateRequested(flipperInfo: var)
    signal extraOptionsRequested()

    required property var info
    required property string name
    required property string model
    required property string version
    required property string message
    required property real progress

    id: item
    width: parent.width
    height: 80

    StyledProgressBar {
        id: progressBar
        anchors.fill: parent
        anchors.margins: frame.border.width
        value: progress
    }

    Rectangle {
        id: frame
        radius: 4
        anchors.fill: parent
        color: "transparent"
        border.color: "white"
        border.width: 1
    }

    Text {
        id: modelLabel
        text: model
        color: "darkgray"
        font.pointSize: 8
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
    }

    Rectangle {
        id: nameLabel
        color: (name === "DFU Mode") ? "#0345ff" : "darkorange"
        width: 100
        height: 40

        radius: height/2

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: modelLabel.right
        anchors.leftMargin: 10

        Text {
            text: name
            color: "black"
            font.pointSize: 12
            font.bold: true
            anchors.centerIn: parent
        }
    }

    StyledButton {
        id: updateButton
        text: message
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: versionLabel
        text: qsTr("version ") + version
        font.pointSize: 8

        anchors.left: nameLabel.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10

        color: "darkgray"
    }

    Connections {
        target: updateButton
        function onClicked() { updateRequested(info) }
        function onPressAndHold() { extraOptionsRequested() }
    }
}
