import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

Item {
    id: control

    property alias content: content
    property alias text: content.text
    property alias textFormat: content.textFormat
    property alias background: scrollView.background

    property Menu menu

    ScrollView {
        id: scrollView
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        TextEdit {
            id: content
            width: scrollView.availableWidth

            cursorVisible: false
            persistentSelection: true

            font.pixelSize: 16
            font.letterSpacing: -1
            font.family: "Share Tech Mono"

            selectionColor: Theme.color.lightorange2
            selectedTextColor: Theme.color.darkorange1

            color: Theme.color.lightorange2
            wrapMode: Text.Wrap

            readOnly: true
            selectByMouse: true
            selectByKeyboard: true
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.IBeamCursor
        acceptedButtons: Qt.RightButton
        onClicked: if(control.menu && mouse.button === Qt.RightButton) control.menu.popup()
    }

    function scrollToBottom() {
        scrollView.ScrollBar.vertical.increase();
    }
}
