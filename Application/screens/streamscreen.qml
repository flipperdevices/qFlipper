import QtQml 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.12

import QFlipper 1.0

import "../components"

Item {
    id: screen
    anchors.fill: parent

    property var device
    signal homeRequested()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Rectangle {
            radius: 4
            color: "transparent"
            border.width: 1
            border.color: "#2E2E2E"

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScreenCanvas {
                id: screenCanvas
                visible: true
                anchors.fill: parent
                data: device.screenData
            }
        }

        RowLayout {
            spacing: 20

            StyledButton {
                id: saveButton
                text: qsTr("Save Image")
                Layout.fillWidth: true
            }

            StyledButton {
                id: backButton
                text: qsTr("Back")
                Layout.fillWidth: true

                onClicked: {
                    device.screenStream = false;
                    screen.homeRequested();
                }
            }
        }
    }

    Component.onCompleted: {
        device.screenStream = true;
    }
}
