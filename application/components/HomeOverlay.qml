import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

Item {
    id: overlay

    readonly property int centerX: 590
    layer.enabled: true

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 350
        }
    }

    TabPane {
        width: 322

        anchors.top: tabs.bottom
        anchors.left: tabs.left
        anchors.topMargin: -2

        currentIndex: tabs.currentIndex

        items: [
            DeviceInfo {},
            DeviceActions {}
        ]
    }

    TabBar {
        id: tabs
        x: 28
        y: 28


        TabButton {
            icon.source: "qrc:/assets/gfx/symbolic/info.svg"
            icon.width: 25
            icon.height: 25

//            ToolTip.text: qsTr("Device information")
        }

        TabButton {
            icon.source: "qrc:/assets/gfx/symbolic/wrench.svg"
            icon.width: 27
            icon.height: 27

//            ToolTip.text: qsTr("Avdanced controls")
        }
    }

    Text {
        id: nameLabel
        x: centerX - width - 4
        y: 24

        color: Theme.color.orange

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        text: "BabyYoda"
    }

    ColumnLayout {
        x: centerX + 4
        anchors.bottom: nameLabel.baseline

        TransparentLabel {
            id: connectionLabel
            height: 14

            font.capitalization: Font.AllUppercase
            icon.source: "qrc:/assets/gfx/symbolic/usb-connected.svg"
            text: qsTr("Connected")
        }

        TransparentLabel {
            id: systemPathLabel
            height: 14

            text: "/dev/ttyACM3"
        }
    }

    UpdateButton {
        id: updateButton

        x: Math.round(centerX - width / 2)
        y: 260
    }

    LinkButton {
        id: releaseButton
        x: centerX - width - 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 8

        linkColor: Theme.color.lightgreen

        text: "Release 0.42.1"
    }

    LinkButton {
        id: fileButton
        x: centerX + 6

        anchors.top: updateButton.bottom
        anchors.topMargin: 8

        text: "Install from file"
    }
}
