import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

Rectangle {
    id: control

    property string text
    property string style

    property int padding: 20

    implicitHeight: content.implicitHeight + padding * 2
    radius: 6

    color: "black"
    border.color: Theme.color.lightorange2

    Text {
        id: content

        x: control.padding
        y: control.padding

        text: control.style + control.text

        color: Theme.color.lightorange2
        width: control.width - control.padding * 2

        font.pixelSize: 16
        font.letterSpacing: -1
        font.family: "Share Tech Mono"

        padding: 0
        wrapMode: Text.Wrap
        textFormat: Text.RichText

        onLinkActivated: Qt.openUrlExternally(link)
    }
}
