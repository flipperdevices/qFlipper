import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2

import QFlipper 1.0

import "../components"

Item {
    id: screen
    anchors.fill: parent

    property var device
    signal homeRequested()

    function close() {
        screen.homeRequested();
        device.remote.enabled = false;
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file")
        folder: shortcuts.home
        selectExisting: false
        nameFilters: ["PNG images (*.png)", "JPEG images (*.jpg)"]

        onAccepted: {
            screenCanvas.saveImage(fileUrl);
        }
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 16
        columns: 2

        ScreenCanvas {
            id: screenCanvas
            visible: true

            Layout.fillHeight: true
            Layout.fillWidth: true

            canvasWidth: device.remote.screenWidth
            canvasHeight: device.remote.screenHeight

            data: device.remote.screenData
        }

        StyledKeypad {
            id: keypad
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            onInputEvent: {
                device.remote.sendInputEvent(key, type);
            }
        }

        RowLayout {
            spacing: 16
            Layout.columnSpan: 2

            StyledButton {
                id: clipButton
                text: qsTr("Copy to Clipboard")
                Layout.fillWidth: true
                Keys.forwardTo: keypad

                onClicked: {
                    screenCanvas.copyToClipboard();
                }
            }

            StyledButton {
                id: saveButton
                text: qsTr("Save to File")
                Layout.fillWidth: true
                Keys.forwardTo: keypad

                onClicked: {
                    fileDialog.open();
                }
            }

            StyledButton {
                id: backButton
                text: qsTr("Close")
                Layout.fillWidth: true
                Keys.forwardTo: keypad

                onClicked: {
                    screen.close();
                }
            }
        }
    }

    Component.onCompleted: {
        keypad.forceActiveFocus();
        device.remote.enabled = true;
    }
}
