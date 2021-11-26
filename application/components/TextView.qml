import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

ScrollView {
    id: control

    property alias content: content
    property alias text: content.text
    property alias textFormat: content.textFormat

    clip: true
    contentWidth: availableWidth

    TextEdit {
        id: content
        width: availableWidth

        cursorVisible: false

        font.pixelSize: 16
        font.letterSpacing: -1
        font.family: "Share Tech Mono"

        selectionColor: Theme.color.lightorange2
        selectedTextColor: Theme.color.darkorange1

        color: Theme.color.lightorange2
        wrapMode: Text.Wrap
    }
}
