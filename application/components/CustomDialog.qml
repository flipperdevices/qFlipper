import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

Popup {
    id: dialog

    signal accepted
    signal rejected

    padding: 0

    width: parent.width
    height: parent.height

    property alias radius: bg.radius

    background: Rectangle {
        id: bg
        color: "black"
        opacity: 0.8
    }

    Component.onCompleted: {
        accepted.connect(close)
        rejected.connect(close)
    }
}
