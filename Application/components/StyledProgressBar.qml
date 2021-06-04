import QtQuick 2.12
import QtQuick.Controls 2.12

ProgressBar {
    id: control
    from: 0
    to: 100

    visible: value != 0

    background: Rectangle {
        color: "transparent"
    }

    contentItem: Item {
        anchors.fill: parent

        Rectangle {
            color: "#26003b"
            height: parent.height
            width: control.visualPosition * parent.width
        }
    }
}
