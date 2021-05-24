import QtQuick 2.0

Rectangle {
    signal flashRequested(serialNumber: string)

    id: item
    width: parent.width
    height: 80
    radius: 4
    color: "black"
    border.color: "white"
    border.width: 1

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
        color: "darkorange"
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
        id: flashButton
        text: qsTr("Update")
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
        target: flashButton
        function onClicked() { flashRequested(serial) }
    }
}
