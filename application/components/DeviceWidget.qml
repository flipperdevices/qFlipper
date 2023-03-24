import QtQuick 2.15
import QtQuick.Controls 2.15

import QFlipper 1.0
import Theme 1.0

Image {
    id: control

    signal screenStreamRequested

    readonly property var deviceState: Backend.deviceState

    visible: opacity > 0
    source: "qrc:/assets/gfx/images/flipper.svg"
    sourceSize: Qt.size(360, 156)

    Behavior on x {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 350
        }
    }

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 350
        }
    }

    AbstractButton {
        id: clickArea
        width: control.width
        height: control.height
        visible: screenCanvas.visible
        onClicked: control.screenStreamRequested()
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

        source: deviceState && deviceState.isRecoveryMode ? "qrc:/assets/gfx/images/recovery.svg" :
                Backend.backendState === ApplicationBackend.Finished ? "qrc:/assets/gfx/images/success.svg" :
                                                                       "qrc:/assets/gfx/images/default.svg"
        sourceSize: Qt.size(128, 64)
    }

    ScreenCanvas {
        id: screenCanvas
        anchors.fill: defaultScreen
        visible: Backend.screenStreamer.isEnabled &&
                 Backend.backendState > ApplicationBackend.WaitingForDevices &&
                 Backend.backendState < ApplicationBackend.ScreenStreaming

        foregroundColor: Theme.color.darkorange1
        backgroundColor: Theme.color.lightorange2

        frame: Backend.screenStreamer.screenFrame
    }

    ExpandWidget {
        id: expandWidget

        x: 89
        y: 22

        width: 136
        height: 73

        visible: screenCanvas.visible
        opacity: clickArea.hovered ? clickArea.down ? 0.9 : 1 : 0
    }
}
