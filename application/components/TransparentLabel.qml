import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

Item {
    id: control

    property int margins: 3
    property color color: Theme.color.lightgreen

    property alias text: label.text
    property alias font: label.font
    property alias icon: icon

    font.family: "Terminus (TTF)"
    font.pixelSize: 12

    implicitWidth: layout.implicitWidth + margins * 2

    Rectangle {
        id: bg
        anchors.fill: parent
        color: Color.transparent(control.color, 0.2)
    }

    RowLayout {
        id: layout
        x: margins

        anchors.top: control.top
        anchors.bottom: control.bottom

        Text {
            id: label
            color: control.color

            verticalAlignment: Qt.AlignVCenter
            Layout.fillHeight: true
        }

        IconImage {
            id: icon
            color: control.color
            sourceSize: Qt.size(width, height)
        }
    }
}
