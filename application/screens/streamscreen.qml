import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
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

        property var acceptedFunc
        onAccepted: acceptedFunc()
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

                onPressAndHold: {
                    scaleMenu.actionFunc = function(scale) { screenCanvas.copyToClipboard(scale) };
                    scaleMenu.popupCentered(clipButton)
                }
            }

            StyledButton {
                id: saveButton
                text: qsTr("Save to File")
                Layout.fillWidth: true
                Keys.forwardTo: keypad

                onClicked: {
                    fileDialog.acceptedFunc = function() { screenCanvas.saveImage(fileDialog.fileUrl, 0) };
                    fileDialog.open();
                }

                onPressAndHold: {
                    scaleMenu.actionFunc = function (scale) {
                        fileDialog.acceptedFunc = function() { screenCanvas.saveImage(fileDialog.fileUrl, scale) };
                        fileDialog.open();
                    };

                    scaleMenu.popupCentered(saveButton);
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

            Menu {
                id: scaleMenu
                property var actionFunc

                MenuItem {
                    text: qsTr("20X - 2560x1280 px")
                    onClicked: scaleMenu.actionFunc(20)
                }

                MenuItem {
                    text: qsTr("5X - 640x320 px")
                    onClicked: scaleMenu.actionFunc(5)
                }

                MenuItem {
                    text: qsTr("Original - 128x64 px")
                    onClicked: scaleMenu.actionFunc(1)
                }

                function popupCentered(parentItem) {
                    popup(parentItem, parentItem.pressX - width / 2, 0);
                }
            }
        }
    }

    Component.onCompleted: {
        keypad.forceActiveFocus();
        device.remote.enabled = true;
        device.isOnlineChanged.connect(screen.close);
    }
}
