import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import Theme 1.0

Item {
    id: overlay
    visible: opacity > 0

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 150
        }
    }

    Image {
        id: usbPlug

        x: parent.width - 216
        y: 145

        source: "qrc:/assets/gfx/images/typec.svg"
        sourceSize: Qt.size(159, 37)
    }

    TextLabel {
        id: connectMsg
        anchors.horizontalCenter: parent.horizontalCenter
        y: 264

        color: Theme.color.lightorange2
        text: qsTr("Connect your Flipper")

        font.family: "Born2bSportyV2"
        font.pixelSize: 48
    }
}
