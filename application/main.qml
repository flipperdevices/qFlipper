import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import Theme 1.0

import "components"

Window {
    id: root
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint
    title: Qt.application.displayName

    readonly property int shadowSize: 16
    readonly property int shadowOffset: 4

    minimumWidth: mainWindow.width + shadowSize * 2
    minimumHeight: mainWindow.height + shadowSize * 2

    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    color: "transparent"

    Rectangle {
        id: bounds
        anchors.fill: parent
        color: "transparent"
        border.color: Qt.rgba(1, 1, 1, 0.05)
        border.width: 1
    }

    DragHandler {
        onActiveChanged: if(active) { root.startSystemMove(); }
        target: null
    }

    Rectangle {
        id: mainWindow

        x: shadowSize
        y: shadowSize - shadowOffset

        width: 830
        height: 500

        radius: 10

        color: "black"
        border.color: Theme.color.mediumorange3
        border.width: 2

        layer.enabled: true
        layer.effect: DropShadow {
            cached: true
            radius: shadowSize
            samples: shadowSize * 2 + 1
            horizontalOffset: 0
            verticalOffset: shadowOffset
            color: Qt.rgba(0, 0, 0, 0.3)
        }

        states: State {
            name: "connected"
            when: logButton.checked

            PropertyChanges {
                target: device
                x: Math.floor(mainContent.width / 2)
            }

            PropertyChanges {
                target: noDeviceOverlay
                opacity: 0
            }

            PropertyChanges {
                target: homeOverlay
                opacity: 1
            }
        }

        MainBackground {
            id: mainContent

            anchors.horizontalCenter: parent.horizontalCenter
            y: 38

            width: 800 + border.width * 2
            height: 390 + border.width * 2

            NoDeviceOverlay {
                id: noDeviceOverlay
                anchors.fill: parent
            }

            HomeOverlay {
                id: homeOverlay
                anchors.fill: parent
                opacity: 0
            }

            Image {
                id: device

                x: 216
                y: 80

                source: "qrc:/assets/gfx/images/flipper.svg"

                Behavior on x {
                    PropertyAnimation {
                        easing.type: Easing.InOutQuad
                        duration: 350
                    }
                }
            }
        }

        RowLayout {
            id: footerLayour
            width: mainContent.width

            height: 42
            spacing: 15

            anchors.horizontalCenter: mainContent.horizontalCenter
            anchors.top: mainContent.bottom
            anchors.topMargin: 13

            Button {
                id: logButton
                text: qsTr("LOGS")

                Layout.preferredWidth: 110
                Layout.fillHeight: true

                icon.width: 24
                icon.height: 24
                icon.source: checked ? "qrc:/assets/gfx/symbolic/arrow-up.svg" :
                                       "qrc:/assets/gfx/symbolic/arrow-down.svg"

                checkable: true
            }

            Rectangle {
                id: popupBg
                color: Theme.color.brown

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: popupText
                    text: "New firmware release 0.42.1 is available"

                    anchors.fill: parent
                    anchors.leftMargin: 10
                    color: Theme.color.red

                    font.capitalization: Font.AllUppercase
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
