import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2

import QFlipper 1.0
import Theme 1.0

AbstractOverlay {
    id: overlay

    signal closeRequested

    FileDialog {
        id: fileDialog

        title: qsTr("Please choose a file")
        folder: shortcuts.home
        selectExisting: false

        nameFilters: ["PNG images (*.png)", "JPEG images (*.jpg)"]
        onAccepted: canvas.saveImage(fileUrl, 4)
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

            icon.height: 20
            icon.width: 20
            icon.source: "qrc:/assets/gfx/symbolic/save-symbolic.svg"
        }
    }

    DirectionalKeypad {
        id: keypad

        x: 590
        y: 52

        focus: enabled

        onInputEvent: {
            device.streamer.sendInputEvent(key, type);
        }

    }

    IconImage {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 14

        color: Theme.color.lightorange2
        source: "qrc:/assets/gfx/symbolic/info-big.svg"
        sourceSize: Qt.size(32, 32)

        Control {
            anchors.fill: parent

            ToolTip {
                delay: 100
                timeout: -1
                visible: parent.hovered

                contentItem: Image {
                    source: "qrc:/assets/gfx/images/streaming-help.svg"
                    sourceSize: Qt.size(207, 80)
                }
            }
        }
    }

    Action {
        id: backAction
        text: qsTr("Back")
        enabled: overlay.enabled
        onTriggered: opacity = 0
    }

    Action {
        id: saveAction
        text: qsTr("Save Screenshot")
        shortcut: "Ctrl+S"
        enabled: overlay.enabled
        onTriggered: fileDialog.open()
    }

//    Action {
//        id: copyAction
//        text: qsTr("Copy to clipboard")
//        shortcut: "Ctrl+C"
//        enabled: overlay.enabled
//        onTriggered: canvas.copyToClipboard(resolutionCombo.currentValue)
//    }

}
