import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2

import QFlipper 1.0
import Theme 1.0

Item {
    id: overlay

    signal closeRequested

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

    visible: opacity > 0
    enabled: visible

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 150
        }
    }

    FileDialog {
        id: fileDialog

        title: qsTr("Please choose a file")
        folder: shortcuts.home
        selectExisting: false

        nameFilters: ["PNG images (*.png)", "JPEG images (*.jpg)"]
        onAccepted: canvas.saveImage(fileUrl, resolutionCombo.currentValue)
    }

    Rectangle {
        id: canvasBg
        anchors.fill: canvas
        anchors.margins: -11

        radius: 10
        color: Theme.color.lightorange2
        border.color: Theme.color.mediumorange1
        border.width: 3
    }

    ScreenCanvas {
        id: canvas

        x: 38
        y: 32

        foregroundColor: "black"//Theme.color.darkorange1
        backgroundColor: Theme.color.lightorange2

        canvasWidth: device ? device.streamer.screenWidth : 1
        canvasHeight: device ? device.streamer.screenHeight : 1

        width: canvasWidth * 4
        height: canvasHeight * 4

        data: !!device && enabled ? device.streamer.screenData : ""
    }

    RowLayout {
        id: buttonLayout
        x: canvasBg.x
        width: canvasBg.width
        spacing: 20

        anchors.bottom: overlay.bottom
        anchors.bottomMargin: 22

        Button {
            action: backAction
            icon.source: "qrc:/assets/gfx/symbolic/arrow-back.svg"
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            action: saveAction
            Layout.alignment: Qt.AlignRight
        }

        Button {
            action: copyAction
            Layout.alignment: Qt.AlignRight
        }
    }

    ComboBox {
        id: resolutionCombo

        model: [
            { scale: 1, text: qsTr("128x64 px") },
            { scale: 5, text: qsTr("640x320 px") },
            { scale: 20, text: qsTr("2560x1280 px") }
        ]

        textRole: "text"
        valueRole: "scale"

        anchors.top: buttonLayout.top

        anchors.left: buttonLayout.right
        anchors.leftMargin: buttonLayout.spacing

        anchors.right: overlay.right
        anchors.rightMargin: canvasBg.x
    }

    Action {
        id: backAction
        text: qsTr("Back")
        enabled: overlay.enabled
        onTriggered: closeRequested()
    }

    Action {
        id: saveAction
        text: qsTr("Save as")
        shortcut: "Ctrl+S"
        enabled: overlay.enabled
        onTriggered: fileDialog.open()
    }

    Action {
        id: copyAction
        text: qsTr("Copy to clipboard")
        shortcut: "Ctrl+C"
        enabled: overlay.enabled
        onTriggered: canvas.copyToClipboard(resolutionCombo.currentValue)
    }

}
