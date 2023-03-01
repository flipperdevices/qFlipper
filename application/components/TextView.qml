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

        Flickable {
            id: flickView
            boundsBehavior: Flickable.StopAtBounds
            contentWidth: scrollView.contentWidth;
            contentHeight: content.implicitHeight;

            TextEdit {
                id: content
                width: flickView.contentWidth

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

                onTextChanged: scrollToBottom();
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.IBeamCursor
        acceptedButtons: Qt.RightButton
        onClicked: function(mouse) {
            if(control.menu && mouse.button === Qt.RightButton) control.menu.popup();
        }
    }

    onVisibleChanged: if(visible) scrollToBottom()

    function scrollToBottom() {
        if(visible) {
            const sb = scrollView.ScrollBar.vertical;
            sb.position = 1.0 - sb.size;
        }
    }
}
