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

        canvasWidth: Backend.screenStreamer.screenSize.width
        canvasHeight: Backend.screenStreamer.screenSize.height

        width: canvasWidth * 4
        height: canvasHeight * 4

        data: Backend.screenStreamer.screenData
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

        onInputEvent: {
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
        onTriggered: Backend.stopFullScreenStreaming()
    }

    Action {
        id: saveAction
        text: qsTr("Save Screenshot")
        shortcut: "Ctrl+S"
        enabled: overlay.enabled

        onTriggered: {
            const onFinished = function() {
                AdvancedFileDialog.accepted.disconnect(onAccepted);
                AdvancedFileDialog.finished.disconnect(onFinished);
            };

            const onAccepted = function() {
                const ext = AdvancedFileDialog.selectedNameFilter.match("\\.\\w+")[0];
                let strurl = AdvancedFileDialog.fileUrl.toString();

                if(!strurl.endsWith(ext)) {
                    strurl += ext;
                }

                canvas.saveImage(Qt.resolvedUrl(strurl), 4);
            };

            AdvancedFileDialog.accepted.connect(onAccepted);
            AdvancedFileDialog.finished.connect(onFinished);

            const date = new Date();

            AdvancedFileDialog.defaultFileName = "Screenshot-%1.png".arg(Qt.formatDateTime(date, "yyyyMMdd-hhmmss"));
            AdvancedFileDialog.title = qsTr("Please choose a file");
            AdvancedFileDialog.nameFilters = ["PNG images (*.png)", "JPEG images (*.jpg)"];
            AdvancedFileDialog.openLocation = AdvancedFileDialog.PicturesLocation;
            AdvancedFileDialog.selectMultiple = false;
            AdvancedFileDialog.selectExisting = false;
            AdvancedFileDialog.selectFolder = false;

            AdvancedFileDialog.exec();
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
