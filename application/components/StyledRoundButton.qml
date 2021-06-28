import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle {
    property alias icon: button.icon.source

    width: 50
    height: width
    radius: width / 2

    color: "#222"
    border.color: "#2E2E2E"
    border.width: 1

    StyledToolButton {
        id: button
        anchors.fill: parent
    }
}
