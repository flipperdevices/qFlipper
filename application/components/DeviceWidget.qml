import QtQuick 2.15
import QtQuick.Controls 2.15

import QFlipper 1.0
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
        visible: !!deviceState && deviceState.isRecoveryMode

        x: 234
        y: 90

        width: 9
        height: width

        radius: Math.round(width / 2)
        color: Theme.color.lightblue
    }

    Image {
        id: defaultScreen

        x: 93
        y: 26

        visible: !device || !device.streamer.enabled
        source: deviceState && deviceState.isRecoveryMode ? "qrc:/assets/gfx/images/recovery.svg" : "qrc:/assets/gfx/images/default.svg"
        sourceSize: Qt.size(128, 64)
    }

    ScreenCanvas {
        id: screenCanvas
        anchors.fill: defaultScreen
        visible: !!device && device.streamer.enabled

        foregroundColor: Theme.color.darkorange1
        backgroundColor: Theme.color.lightorange2

        canvasWidth: device ? device.streamer.screenWidth : 1
        canvasHeight: device ? device.streamer.screenHeight : 1

        data: device ? device.streamer.screenData : ""
    }
}
