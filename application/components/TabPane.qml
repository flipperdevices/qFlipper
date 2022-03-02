import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

ColumnLayout {
    id: control
    spacing: 0

    property alias currentIndex: content.currentIndex
    property list<Item> items

    property int radius: 7
    property int borderWidth: 2

    property color backgroundColor: Qt.rgba(0,0,0,0)

    Behavior on backgroundColor {
        ColorAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    Item {
        id: header
        clip: true
        height: parent.radius + parent.borderWidth

        Layout.fillWidth: true


        Rectangle {
            color: control.backgroundColor
            width: parent.width + control.radius
            height: control.radius * 2 + control.borderWidth * 2
            radius: control.radius
            border.color: Theme.color.lightorange2
            border.width: borderWidth
            anchors.right: parent.right
        }

        Item {
            clip: true
            anchors.fill: parent
            anchors.rightMargin: parent.width / 2

            Rectangle {
                color: control.backgroundColor
                width: parent.width + border.width
                height: control.radius + control.borderWidth * 2
                border.color: Theme.color.lightorange2
                border.width: borderWidth
            }
        }
    }

    Item {
        clip: true

        implicitWidth: content.width + control.borderWidth * 2
        implicitHeight: content.height

        Canvas {
            anchors.fill: parent

            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();

                const inset = control.borderWidth;
                ctx.fillStyle = control.backgroundColor;
                ctx.fillRect(inset, 0, width - inset, height);

                ctx.globalAlpha = 0.5;
                ctx.lineDashOffset = 0.5;
                ctx.strokeStyle = Theme.color.lightorange2;
                ctx.lineWidth = control.borderWidth * 2;
                ctx.setLineDash([0.5, 1.25]);

                ctx.moveTo(0,0);
                ctx.lineTo(0, height);

                ctx.moveTo(width, 0);
                ctx.lineTo(width, height)

                ctx.stroke();
            }

            onVisibleChanged: if(visible) requestPaint();
        }

        StackLayout {
            id: content
            x: control.borderWidth

            children: items

            width: children[currentIndex].implicitWidth
            height: children[currentIndex].implicitHeight

            Behavior on width {
                PropertyAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }

            Behavior on height {
                PropertyAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    Item {
        id: footer
        clip: true
        height: parent.radius + parent.borderWidth

        Layout.fillWidth: true

        Rectangle {
            color: control.backgroundColor
            width: parent.width
            height: control.radius * 2 + control.borderWidth * 2
            radius: control.radius
            border.color: Theme.color.lightorange2
            border.width: borderWidth
            anchors.bottom: parent.bottom
        }
    }
}
