import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

Item {
    id: overlay
    visible: opacity > 0
    layer.enabled: true

    property Rectangle backgroundRect

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 150
        }
    }

    Text {
        id: updateLabel
        anchors.horizontalCenter: parent.horizontalCenter
        y: 24

        color: Theme.color.orange

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        text: qsTr("Updating your Flipper")
    }

    ProgressBar {
        id: progressBar

        width: 280
        height: 56

        from: 0
        to: 100

        x: Math.round((parent.width - width) / 2)
        y: 265

        value: deviceState ? deviceState.progress : 0
        indeterminate: !deviceState ? true : deviceState.progress < 0
    }

    Text {
        id: messageLabel
        anchors.top: progressBar.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        text: !deviceState ? text : deviceState.isError ? deviceState.errorString : deviceState.statusString
        font.capitalization: Font.AllUppercase
        color: Theme.color.orange
    }
}
