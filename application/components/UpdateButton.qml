import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

import "../style"

Button {
    id: control

    width: 270
    height: 50

    radius: 7
    borderWidth: 3

    text: qsTr("Update")
    font.family: "Born2bSportyV2"
    font.pixelSize: 48

    foregroundColor: ColorGroup {
        normal: Theme.color.green
        hover: Theme.color.lightgreen
        down: Theme.color.darkgreen
    }

    backgroundColor: ColorGroup {
        normal: Color.transparent(Theme.color.lightgreen, 0.15)
        hover: Color.transparent(Theme.color.lightgreen, 0.3)
        down: Theme.color.green
    }

    strokeColor: ColorGroup {
        normal: Theme.color.green
        hover: Theme.color.lightgreen
        down: Theme.color.green
    }
}
