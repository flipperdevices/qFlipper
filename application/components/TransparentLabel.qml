import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

Item {
    id: control

    property int margins: 3
    property color color: Theme.color.lightgreen

    property alias capitalized: label.capitalized
    property alias text: label.text
    property alias font: label.font
    property alias icon: icon

    font.family: "ProggySquareTT"
    font.pixelSize: 16

    implicitHeight: 15
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


        TextLabel {
            id: label
            color: control.color
            Layout.topMargin: 1
        }

        IconImage {
            id: icon
            color: control.color
            sourceSize: Qt.size(width, height)
            Layout.bottomMargin: 1
        }
    }

    Component.onCompleted: {
        layout.spacing = icon.source != "" ? layout.spacing : 0
    }
}
