import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

Image {
    id: control

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

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
        visible: deviceState ? deviceState.isRecoveryMode : false

        x: 234
        y: 90

        width: 9
        height: width

        radius: Math.round(width / 2)
        color: Theme.color.lightblue
    }

    Image {
        x: 93
        y: 26

        visible: !deviceState || deviceState.isRecoveryMode
        source: !deviceState ? "" : "qrc:/assets/gfx/images/recovery.svg"
        sourceSize: Qt.size(128, 64)
    }
}
