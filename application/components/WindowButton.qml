import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

ToolButton {
    id: control

    property string iconPath
    property string iconName

    icon.width: 20
    icon.height: 20

    implicitWidth: icon.width + padding * 2
    implicitHeight: icon.height + padding * 2

    padding: 0

    background: Item {}

    contentItem: Item {
        width: icon.width
        height: icon.height

        IconImage {
            id: iconNormal
            source: "%1/%2.svg".arg(iconPath).arg(iconName)
        }

        IconImage {
            id: iconHover
            source: "%1/%2_hover.svg".arg(iconPath).arg(iconName)
            opacity: control.hovered ? 1 : 0

            Behavior on opacity {
                PropertyAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
        }

        IconImage {
            id: iconDown
            source: "%1/%2_down.svg".arg(iconPath).arg(iconName)
            opacity: control.down ? 1 : 0
        }
    }
}
