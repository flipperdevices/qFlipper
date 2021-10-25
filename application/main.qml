import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import Theme 1.0

import "components"
import "screens"
import "style"

Window {
    id: root
    visible: true
    flags: Qt.FramelessWindowHint
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
        border.color: Theme.color.darkbrown
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

        Rectangle {
            id: mainContent
            anchors.fill: parent
            anchors.topMargin: 47
            anchors.bottomMargin: 61
            anchors.leftMargin:  15
            anchors.rightMargin: 15

            radius: 9
            border.width: 2
            border.color: Theme.color.orange

            gradient: Gradient {
                GradientStop {position: 0; color: "#090400"}
                GradientStop {position: 1; color: "#210F00"}
            }

            Canvas {
                anchors.fill: parent
                anchors.margins: mainContent.border.width

                opacity: 0.15

                onPaint: {
                    const numCells = 30;
                    const cellSize = width / numCells;

                    const ctx = getContext("2d");
                    ctx.strokeStyle = "#aa5115";
                    ctx.lineWidth = 2;

                    for(let ypos = cellSize; ypos < height; ypos += cellSize) {
                        const pos = Math.floor(ypos);
                        ctx.moveTo(0, pos);
                        ctx.lineTo(width, pos);
                    }

                    for(let xpos = cellSize; xpos < width; xpos += cellSize) {
                        const pos = Math.floor(xpos);
                        ctx.moveTo(pos, 0);
                        ctx.lineTo(pos, height);
                    }

                    ctx.stroke();
                }
            }

            Text {
                id: connectMsg
                anchors.top: mainContent.top
                anchors.topMargin: 30
                anchors.horizontalCenter: mainContent.horizontalCenter

                color: Theme.color.orange
                text: qsTr("Connect your Flipper")
                font.family: "Born2bSportyV2"
                font.pixelSize: 48
            }

            Item {
                id: device

                x: mainContent.width / 2 - deviceBg.width / 2
                y: 100

                Image {
                    id: deviceBg
                    smooth: false
                    source: "qrc:/assets/gfx/images/flipper.svg"
                }

                Behavior on x {
                    PropertyAnimation {
                        easing.type: Easing.InOutQuad
                        duration: 350
                    }
                }
            }

            UpdateButton {
                id: updateButton

                x: 460
                y: 265
            }

            Button {
                text: "BUMP"
                x: 20
                y: 150

                checkable: true

                onCheckedChanged: {
                    if(checked) {
                        device.x = mainContent.width / 2
                    } else {
                        device.x = mainContent.width / 2 - deviceBg.width / 2
                    }
                }
            }
        }

        RowLayout {
                width: mainContent.width
                height: 40
                spacing: 15

                anchors.left: mainWindow.left
                anchors.right: mainWindow.right
                anchors.bottom: mainWindow.bottom

                anchors.leftMargin: 15
                anchors.rightMargin: 15
                anchors.bottomMargin: 11

            Button {
                id: logButton
                text: qsTr("LOGS")

                Layout.preferredWidth: 130
                Layout.fillHeight: true

                icon.source: "qrc:/assets/gfx/symbolic/arrow-dropdown.svg"
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
                    color: Theme.color.orange

                    font.capitalization: Font.AllUppercase
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
