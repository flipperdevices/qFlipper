import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import Theme 1.0

Item {
    id: overlay
    visible: opacity > 0
    layer.enabled: true

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 150
        }
    }

    Image {
        id: usbPlug

        x: parent.width - 200
        y: 175

        source: "qrc:/assets/gfx/images/typec.svg"
        sourceSize: Qt.size(160, 42)
    }

    Text {
        id: connectMsg
        anchors.horizontalCenter: parent.horizontalCenter
        y: 264

        color: Theme.color.lightorange2
        text: qsTr("Connect your Flipper")

        font.capitalization: Font.AllUppercase
        font.family: "Born2bSportyV2"
        font.pixelSize: 48
    }
}
