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

        function openWithArgs(args) {
            const onDialogAccepted = function() {
                onDialogRejected();
                screenCanvas.saveImage(fileUrl, args.scalingType);
            }

            const onDialogRejected = function() {
                accepted.disconnect(onDialogAccepted);
                rejected.disconnect(onDialogRejected);
            }

            accepted.connect(onDialogAccepted);
            rejected.connect(onDialogRejected);
            open();
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

                // TODO: Use only one menu
                Menu {
                    id: clipMenu

                    MenuItem {
                        text: qsTr("4X scaling")
                        onTriggered: screenCanvas.copyToClipboard(ScreenCanvas.Scaling4X);
                    }

                    MenuItem {
                        text: qsTr("3X scaling")
                        onTriggered: screenCanvas.copyToClipboard(ScreenCanvas.Scaling3X);
                    }

                    MenuItem {
                        text: qsTr("2X scaling")
                        onTriggered: screenCanvas.copyToClipboard(ScreenCanvas.Scaling2X);
                    }

                    MenuItem {
                        text: qsTr("No scaling")
                        onTriggered: screenCanvas.copyToClipboard(ScreenCanvas.NoScaling);
                    }
                }

                onPressAndHold: {
                    clipMenu.x = pressX - width / 2;
                    clipMenu.open();
                }
            }

            StyledButton {
                id: saveButton
                text: qsTr("Save to File")
                Layout.fillWidth: true
                Keys.forwardTo: keypad

                // TODO: Use only one menu
                Menu {
                    id: saveMenu

                    MenuItem {
                        text: qsTr("4X scaling")
                        onTriggered: fileDialog.openWithArgs({scalingType: ScreenCanvas.Scaling4X});
                    }

                    MenuItem {
                        text: qsTr("3X scaling")
                        onTriggered: fileDialog.openWithArgs({scalingType: ScreenCanvas.Scaling3X});
                    }

                    MenuItem {
                        text: qsTr("2X scaling")
                        onTriggered: fileDialog.openWithArgs({scalingType: ScreenCanvas.Scaling2X});
                    }

                    MenuItem {
                        text: qsTr("No scaling")
                        onTriggered: fileDialog.openWithArgs({scalingType: ScreenCanvas.NoScaling});
                    }
                }

                onClicked: {
                    fileDialog.openWithArgs({scalingType: ScreenCanvas.AsDisplayed});
                }

                onPressAndHold: {
                    saveMenu.x = pressX - width / 2;
                    saveMenu.open();
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
        device.isConnectedChanged.connect(screen.close);
    }
}
