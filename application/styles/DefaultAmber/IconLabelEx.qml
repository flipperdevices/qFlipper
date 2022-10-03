import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Misc 1.0

RowLayout {
    id: control

    property int padding

    property alias icon: image
    property alias text: label.text
    property alias font: label.font
    property alias color: label.color

    spacing: 6

    IconImage {
        id: image
        Layout.leftMargin: control.padding
    }

    Text {
        id: label

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.rightMargin: control.padding

        antialiasing: Mitigations.fontRenderingFix
    }
}
