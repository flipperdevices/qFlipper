import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

Item {
    id: control

    property color color: Theme.color.lightorange2
    clip: true

    Rectangle {
        id: scanline
        width: parent.width
        height: 320
        gradient: Gradient {
            GradientStop {position: 0;     color: Color.transparent(control.color, 0)}
            GradientStop {position: 0.6;   color: Color.transparent(control.color, 0.01)}
            GradientStop {position: 0.99;  color: Color.transparent(control.color, 0.07)}
            GradientStop {position: 1;     color: Color.transparent(control.color, 0)}
        }
    }

    PropertyAnimation {
        target: scanline

        property: "y"
        duration: 4000

        from: -scanline.height
        to: control.height

        loops: Animation.Infinite
        easing.type: Easing.Linear
        running: true
    }
}
