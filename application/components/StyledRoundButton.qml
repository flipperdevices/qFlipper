import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle {
    id: control
    property alias icon: button.icon

    signal pressed
    signal released
    signal shortPress
    signal longPress
    signal repeat

    function setPressed() { button.setPressed() }
    function setReleased() { button.setReleased() }

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
        onShortPress: control.shortPress()
        onLongPress: control.longPress()
        onRepeat: control.repeat()
    }
}
