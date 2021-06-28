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
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }

        Item {
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 20
            Layout.alignment: Qt.AlignRight

            StyledButton {
                id: saveButton
                text: qsTr("Save Image")

                onClicked: {
                    fileDialog.open();
                }
            }

            StyledButton {
                id: backButton
                text: qsTr("Close")

                onClicked: {
                    device.remote.enabled = false;
                    screen.homeRequested();
                }
            }
        }
    }

    Component.onCompleted: {
        device.remote.enabled = true;
    }
}
