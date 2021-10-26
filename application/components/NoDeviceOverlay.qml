import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

Item {
    id: noDeviceOverlay

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 150
        }
    }

    Image {
        id: usbPlug

        x: parent.width - 200
        y: 170

        source: "qrc:/assets/gfx/images/typec.svg"

    }

    Text {
        id: connectMsg
        anchors.horizontalCenter: parent.horizontalCenter
        y: 255

        color: Theme.color.orange
        text: qsTr("Connect your Flipper")

        font.capitalization: Font.AllUppercase
        font.family: "Born2bSportyV2"
        font.pixelSize: 48
    }
}
