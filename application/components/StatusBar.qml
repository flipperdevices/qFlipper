import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0
import QFlipper 1.0

Rectangle {
    id: control

    readonly property bool errorOccured : Logger.errorCount > 0
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

            text: control.errorOccured ? qsTr("Something went wrong. Check logs for details.") :
                  Backend.state === Backend.WaitingForDevices ? qsTr("Waiting for devices ...") :
                  Backend.state > Backend.ScreenStreaming && Backend.state < Backend.Finished ? qsTr("Do not unplug the device ...") : qsTr("Ready.")

            color: control.errorOccured ? Theme.color.lightred1 : Theme.color.lightorange2

            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }
    }
}
