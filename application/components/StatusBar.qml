import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0
import QFlipper 1.0

Rectangle {
    id: control

    readonly property bool errorOccured: Backend.state === Backend.ErrorOccured ||
                                         Backend.firmwareUpdateStatus === Backend.ErrorOccured

    color: errorOccured ? Theme.color.darkred2 : Theme.color.darkorange1

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        spacing: 10

        IconImage {
            color: message.color
            sourceSize: Qt.size(22, 22)
            source: "qrc:/assets/gfx/symbolic/info-small.svg"
            rotation: errorOccured ? 180 : 0
        }

        TextLabel {
            id: message

            text: Backend.state === Backend.ErrorOccured ? qsTr("Something went wrong. Check logs for details.") :
                  Backend.state === Backend.WaitingForDevices ? qsTr("Waiting for devices ...") :
                  Backend.state > Backend.ScreenStreaming && Backend.state < Backend.Finished ? qsTr("Do not unplug the device ...") :
                  Backend.state === Backend.Finished ? qsTr("Operation has finished successfully.") :
                  Backend.firmwareUpdateStatus === Backend.ErrorOccured ? qsTr("Cannot connect to update server.") :
                  Backend.firmwareUpdateStatus === Backend.Checking ? qsTr("Checking for firmware updates...") : qsTr("Ready.")

            color: control.errorOccured ? Theme.color.lightred4 : Theme.color.lightorange2

            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }
    }

    Behavior on color {
        ColorAnimation {
            duration: 150
            easing.type: Easing.OutQuad
        }
    }
}
