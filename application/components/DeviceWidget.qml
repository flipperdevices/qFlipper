import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

Image {
    id: control

    property bool recoveryMode: false

    source: "qrc:/assets/gfx/images/flipper.svg"
    sourceSize: Qt.size(360, 156)

    Behavior on x {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 350
        }
    }

    Rectangle {
        id: blueLed
        visible: recoveryMode

        x: 234
        y: 90

        width: 9
        height: width

        radius: Math.round(width / 2)
        color: Theme.color.lightblue
    }
}
