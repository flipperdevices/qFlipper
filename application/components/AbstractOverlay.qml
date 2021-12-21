import QtQuick 2.15

import QFlipper 1.0

Item {
    id: overlay

    property Rectangle backgroundRect

    readonly property var device: Backend.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

    property alias fadeAnimation: fadeAnimation

    visible: opacity > 0
    enabled: visible
    fadeAnimation.duration: 150

    Behavior on opacity {
        PropertyAnimation {
            id: fadeAnimation
            easing.type: Easing.InOutQuad
        }
    }
}
