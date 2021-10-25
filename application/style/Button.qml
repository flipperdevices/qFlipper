import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T

import Theme 1.0

T.Button {
    id: control

    property var foregroundColor: ColorGroup {
        normal: Theme.color.orange
        hover: Theme.color.lightorange
        down: Theme.color.brown
    }

    property var backgroundColor: ColorGroup {
        normal: Theme.color.brown
        hover: Theme.color.darkorange
        down: Theme.color.orange
    }

    property var strokeColor: ColorGroup {
        normal: Theme.color.orange
        hover: Theme.color.lightorange
        down: Theme.color.orange
    }

    property alias radius: bg.radius
    property alias borderWidth: bg.border.width

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    horizontalPadding: padding + 2
    spacing: 6

    icon.width: 16
    icon.height: 16
    icon.color: foregroundColor.normal

    font.capitalization: Font.AllUppercase

    contentItem: IconLabel {
        id: content
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font

        color: foregroundColor.normal
    }

    background: Rectangle {
        id: bg

        implicitWidth: 100
        implicitHeight: 40

        radius: 5

        color: backgroundColor.normal
        border.color: strokeColor.normal
        border.width: 2

        Behavior on color {
            ColorAnimation {
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
    }

    states: [
        State {
            name: "down"
            when: control.down

            PropertyChanges {
                target: bg
                color: backgroundColor.down
                border.color: strokeColor.down
            }

            PropertyChanges {
                target: content
                color: foregroundColor.down
                icon.color: foregroundColor.down
            }
        },

        State {
            name: "hovered"
            when: control.hovered

            PropertyChanges {
                target: bg
                color: backgroundColor.hover
                border.color: strokeColor.hover
            }

            PropertyChanges {
                target: content
                color: foregroundColor.hover
                icon.color: foregroundColor.hover
            }
        }
    ]
}

