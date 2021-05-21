import QtQuick 2.0

Rectangle {
    property string model: modelData.model
    property string name: modelData.name
    property string version: modelData.version
    property string serialNumber: modelData.serialNumber

    signal flashRequested(var serialNumber)

    id: item
    width: parent.width
    height: 80
    radius: 4
    color: "black"
    border.color: "white"
    border.width: 1

    Text {
        id: modelLabel
        text: item.model
        color: "darkgray"
        font.pointSize: 8
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
    }

    Rectangle {
        id: nameLabel
        color: "darkorange"
        width: 100
        height: 40

        radius: height/2

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: modelLabel.right
        anchors.leftMargin: 10

        Text {
            text: item.name
            color: "black"
            font.pointSize: 14
            font.bold: true
            anchors.centerIn: parent
        }
    }

    CoolButton {
        id: flashButton
        text: "Flash"
        anchors.right: parent.right
        anchors.rightMargin: 25
        anchors.verticalCenter: parent.verticalCenter

        onClicked: flashRequested(parent.serialNumber)
    }

    Text {
        id: versionLabel
        text: "version " + item.version
        font.pointSize: 8

        anchors.left: nameLabel.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10

        color: "darkgray"
    }
}
