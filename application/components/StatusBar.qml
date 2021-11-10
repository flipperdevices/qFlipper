import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

Rectangle {
    id: control

    property var deviceState

    color: (deviceState && deviceState.isError) ? Theme.color.darkred2 : Theme.color.darkorange1

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        spacing: 10

        IconImage {
            color: message.color
            source: "qrc:/assets/gfx/symbolic/info-small.svg"
        }

        Text {
            id: message
            text: !deviceState ? qsTr("Waiting for devices ...") :
                   deviceState.isError ? qsTr("Something went wrong. Check logs for details.") :
                   deviceState.isPersistent? qsTr("Do not unplug the device ...") : qsTr("Ready.")

            color: (deviceState && deviceState.isError) ? Theme.color.lightred1 : Theme.color.lightorange2

            font.capitalization: Font.AllUppercase
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }
    }
}
