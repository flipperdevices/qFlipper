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

    Item {
        id: header
        clip: true
        height: parent.radius + parent.borderWidth

        Layout.fillWidth: true

        Item {
            clip: true
            anchors.fill: parent
            anchors.rightMargin: parent.width / 2

            Rectangle {
                color: "transparent"
                width: parent.width + border.width
                height: control.radius + control.borderWidth * 2
                border.color: Theme.color.orange
                border.width: borderWidth
            }
        }

        Rectangle {
            color: "transparent"
            width: parent.width + control.radius
            height: control.radius * 2 + control.borderWidth * 2
            radius: control.radius
            border.color: Theme.color.orange
            border.width: borderWidth
            anchors.right: parent.right
        }
    }

    Item {
        clip: true
        Layout.fillWidth: true
        Layout.preferredHeight: content.height + content.anchors.topMargin * 2

        Canvas {
            anchors.fill: parent

            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();

                ctx.globalAlpha = 0.5;
                ctx.lineDashOffset = 0.5;
                ctx.strokeStyle = Theme.color.orange;
                ctx.lineWidth = control.borderWidth * 2;
                ctx.setLineDash([0.5, 1.25]);

                ctx.moveTo(0,0);
                ctx.lineTo(0, height);

                ctx.moveTo(width, 0);
                ctx.lineTo(width, height)

                ctx.stroke();
            }
        }

        StackLayout {
            id: content
            children: items
            height: children[currentIndex].height

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            anchors.topMargin: 10
            anchors.bottomMargin: anchors.topMargin

            anchors.leftMargin: 20
            anchors.rightMargin: anchors.leftMargin

            Behavior on height {
                PropertyAnimation {
                    duration: 200
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
            color: "transparent"
            width: parent.width
            height: control.radius * 2 + control.borderWidth * 2
            radius: control.radius
            border.color: Theme.color.orange
            border.width: borderWidth
            anchors.bottom: parent.bottom
        }
    }
}
