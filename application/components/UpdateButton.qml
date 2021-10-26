import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

Button {
    id: control

    width: 280
    height: 56

    radius: 7
    borderWidth: 3

    text: qsTr("Update")
    font.family: "Born2bSportyV2"
    font.pixelSize: 48

    foregroundColor: ColorGroup {
        normal: Theme.color.lightgreen
        hover: Theme.color.lightgreen
        down: Theme.color.darkgreen
    }

    backgroundColor: ColorGroup {
        normal: Theme.color.green
        hover: Theme.color.mediumgreen
        down: Theme.color.lightgreen
    }

    strokeColor: ColorGroup {
        normal: Theme.color.lightgreen
        hover: Theme.color.lightgreen
        down: Theme.color.lightgreen
    }
}
