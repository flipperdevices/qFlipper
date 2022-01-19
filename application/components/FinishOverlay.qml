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

        text: Backend.state === Backend.ErrorOccured ? qsTr("Operation Error"): qsTr("Success!")
    }

    TextLabel {
        id: messageLabel

        width: 480
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 33

        text: deviceState ? deviceState.errorString : qsTr("Cannot connect to device")
        visible: Backend.state === Backend.ErrorOccured
    }

    Button {
        id: backButton
        action: backAction

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 25

        icon.width: 24
        icon.height: 24
        icon.source: "qrc:/assets/gfx/symbolic/arrow-back.svg"

        visible: Backend.state === Backend.ErrorOccured
    }

    MainButton {
        id: continueButton
        action: continueAction
        anchors.horizontalCenter: parent.horizontalCenter
        visible: Backend.state === Backend.Finished
        focus: visible
        y: 265

        Keys.onPressed: continueAction.trigger()
    }

    Action {
        id: backAction
        text: qsTr("Back")
        onTriggered: Backend.finalizeOperation()
    }

    Action {
        id: continueAction
        text: qsTr("Continue")
        onTriggered: Backend.finalizeOperation()
    }
}
