import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Layouts 1.15

import QFlipper 1.0
import Theme 1.0

AbstractOverlay {
    id: overlay

    signal closeRequested

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

        foregroundColor: "black"
        backgroundColor: Theme.color.lightorange2
        zoomFactor: 4
        frame: Backend.screenStreamer.screenFrame
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

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:/assets/gfx/symbolic/arrow-back.svg"
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            action: saveAction
            Layout.alignment: Qt.AlignRight

            icon.width: 20
            icon.height: 20
            icon.source: "qrc:/assets/gfx/symbolic/save-symbolic.svg"
        }
    }

    DirectionalKeypad {
        id: keypad

        x: 590
        y: 52

        onInputEvent: function(key, type) {
            Backend.screenStreamer.sendInputEvent(key, type);
        }

        // Prevent focus loss
        onEnabledChanged: focus = enabled
        onActiveFocusChanged: if(!activeFocus) focus = enabled
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
                    source: "qrc:/assets/gfx/images/streaming-help%1.svg".arg(Qt.platform.os === "osx" ? "-mac" : "")
                    sourceSize: Qt.size(207, 102)
                }
            }
        }
    }

    Action {
        id: backAction
        text: qsTr("Back")
        enabled: overlay.enabled
        onTriggered: Backend.stopFullScreenStreaming()
    }

    Action {
        id: saveAction
        text: qsTr("Save Screenshot")
        shortcut: "Ctrl+S"
        enabled: overlay.enabled

        onTriggered: {
            Backend.screenStreamer.isPaused = true;

            SystemFileDialog.finished.connect(function() {
                Backend.screenStreamer.isPaused = false;
            });

            SystemFileDialog.accepted.connect(function() {
                const ext = SystemFileDialog.selectedNameFilter.match("\\.\\w+")[0];
                let strurl = SystemFileDialog.fileUrl.toString();

                if(!strurl.endsWith(ext)) {
                    strurl += ext;
                }

                canvas.saveImage(Qt.resolvedUrl(strurl), 4);
            });

            const defaultName = "Screenshot-%1.png".arg(Qt.formatDateTime(new Date(), "yyyyMMdd-hhmmss"));
            SystemFileDialog.beginSaveFile(SystemFileDialog.PicturesLocation, ["PNG images (*.png)", "JPEG images (*.jpg)"], defaultName);
        }
    }

    Action {
        id: copyAction
        text: qsTr("Copy to clipboard")
        shortcut: StandardKey.Copy
        enabled: overlay.enabled
        onTriggered: canvas.copyToClipboard(4)
    }

}
