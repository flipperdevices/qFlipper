import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

ScrollView {
    id: control

    property alias text: content.text
    property alias textFormat: content.textFormat

    clip: true
    contentWidth: availableWidth

    Text {
        id: content
        width: availableWidth

        lineHeight: 1.2

        font.pixelSize: 16
        font.letterSpacing: -1
        font.family: "Share Tech Mono"

        color: Theme.color.lightorange2
        wrapMode: Text.Wrap
    }
}
