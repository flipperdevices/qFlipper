import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

Item {
    id: control

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.6
        radius: 2
    }

    Item {
        width: 48
        height: 45
        anchors.centerIn: parent

        IconImage {
            anchors.top: parent.top
            anchors.right: parent.right
            source: "qrc:/assets/gfx/symbolic/arrow-expand.svg"
            sourceSize: Qt.size(14, 14)
            color: Theme.color.lightorange1
        }

        IconImage {
            rotation: 180
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            source: "qrc:/assets/gfx/symbolic/arrow-expand.svg"
            sourceSize: Qt.size(14, 14)
            color: Theme.color.lightorange1
        }
    }

    Behavior on opacity {
        PropertyAnimation {
            duration: 150
            easing.type: Easing.InOutQuad
        }
    }
}
