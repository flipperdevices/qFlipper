import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "../components"

Item {
    id: screen
    anchors.fill: parent

    signal homeRequested()

    Text {
        id: titleLabel
        anchors.horizontalCenter: screen.horizontalCenter
        anchors.bottom: container.top
        anchors.bottomMargin: 40

        font.pixelSize: 30
        font.capitalization: Font.AllUppercase

        color: "white"
        text: qsTr("Preferences")
    }

    Text {
        id: subtitleLabel
        anchors.top: titleLabel.bottom
        anchors.horizontalCenter: titleLabel.horizontalCenter
        text: qsTr("Awesome stuff and whatnot")

        color: "darkgray"
        font.capitalization: Font.AllUppercase
    }

    GridLayout {
        id: container
        anchors.top: parent.top
        anchors.topMargin: parent.height/4

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50

        anchors.horizontalCenter: parent.horizontalCenter

        columns: 2
        columnSpacing: 16

        Text {
            color: "white"
            font.pixelSize: 16
            text: qsTr("Flipper update channel")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            id: channelSelector
            implicitWidth: 200
            model: firmwareUpdates.channelNames

            onActivated: {
                console.log(textAt(index))
            }
        }

        Text {
            color: "white"
            font.pixelSize: 16
            text: qsTr("Check for application updates")
            Layout.alignment: Qt.AlignRight
        }

        Switch {
            id: updateSwitch
            checked: true
            Layout.leftMargin: -10
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

    }

    StyledButton {
        id: closeButton
        text: qsTr("Close")

        anchors.right: screen.right
        anchors.bottom: screen.bottom
        anchors.margins: 16

        onClicked: screen.homeRequested()
    }
}
