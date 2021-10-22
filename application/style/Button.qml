import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T

import Theme 1.0

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    horizontalPadding: padding + 2
    spacing: 6

    icon.width: 16
    icon.height: 16
//    icon.color: control.checked || control.highlighted ? control.palette.brightText :
//                control.flat && !control.down ? (control.visualFocus ? control.palette.highlight : control.palette.windowText) : control.palette.buttonText
    icon.color: Theme.color.orange

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
//        color: control.checked || control.highlighted ? control.palette.brightText :
//               control.flat && !control.down ? (control.visualFocus ? control.palette.highlight : control.palette.windowText) : control.palette.buttonText
        color: Theme.color.orange
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        visible: !control.flat || control.down || control.checked || control.highlighted
//        color: Color.blend(control.checked || control.highlighted ? control.palette.dark : control.palette.button,
//                                                                    control.palette.mid, control.down ? 0.5 : 0.0)

        color: Theme.color.brown
        border.color: Theme.color.orange
        border.width: 2
        radius: 5
    }
}

