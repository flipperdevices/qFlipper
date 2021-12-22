import QtQuick 2.15
import QtQuick.Controls 2.15

import QFlipper 1.0
import Theme 1.0

AbstractOverlay {
    id: overlay

    TextLabel {
        id: updateLabel
        capitalized: false
        anchors.horizontalCenter: parent.horizontalCenter
        y: 24

        color: Backend.state === Backend.ErrorOccured ? Theme.color.lightred3 : Theme.color.lightorange2

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        text: Backend.state === Backend.ErrorOccured ? qsTr("Epic fail!"): qsTr("Success!")
    }

    TextLabel {
        anchors.top: parent.top
        anchors.left: parent.left

        anchors.topMargin: 45
        anchors.leftMargin: 35

        text: "Work in progress"
    }

    TextLabel {
        anchors.top: parent.top
        anchors.right: parent.right

        anchors.topMargin: 45
        anchors.rightMargin: 35

        text: "Work in progress"
    }

    TextLabel {
        id: messageLabel

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 105

        text: Backend.state === Backend.Finished ? qsTr("You're good to go!") : deviceState ? deviceState.errorString : qsTr("Cannot connect to device")
    }

    Button {
        id: backButton
        action: backAction

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 35

        icon.width: 24
        icon.height: 24
        icon.source: "qrc:/assets/gfx/symbolic/arrow-back.svg"
    }

    Action {
        id: backAction
        text: qsTr("Back")
        onTriggered: Backend.finalizeOperation()
    }
}
