import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

Item {
    id: control

    property Item contentItem: Item {}

    property int radius: 7
    property int borderWidth: 2

    Item {
        id: header
        clip: true
        width: parent.width
        height: parent.radius + parent.borderWidth
        anchors.top: parent.top

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
        id: content
        width: parent.width
        height: contentItem.height + container.anchors.topMargin * 2
        anchors.top: header.bottom

        Canvas {
            anchors.fill: parent

            onPaint: {
                const ctx = getContext("2d");

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

        Item {
            id: container
            anchors.fill: parent
            anchors.topMargin: 10
            anchors.bottomMargin: anchors.topMargin
            anchors.leftMargin: anchors.topMargin + borderWidth
            anchors.rightMargin: anchors.topMargin + borderWidth
        }

        Component.onCompleted: {
            contentItem.parent = container;
            contentItem.anchors.left = container.left
            contentItem.anchors.right = container.right
        }
    }

    Item {
        id: footer
        clip: true
        width: parent.width
        height: parent.radius + parent.borderWidth
        anchors.top: content.bottom

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
