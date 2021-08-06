import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle {
    id: control
    property alias icon: button.icon.source

    signal pressed()
    signal released()
    signal clicked()
    signal pressAndHold()
    signal repeat()

    width: 50
    height: width
    radius: width / 2

    color: "#222"
    border.color: "#2E2E2E"
    border.width: 1

    StyledToolButton {
        id: button
        anchors.fill: parent

        onPressed: control.pressed()
        onReleased: control.released()
        onClicked: control.clicked()
        onPressAndHold: control.pressAndHold()
        onRepeat: control.repeat()
    }
}
