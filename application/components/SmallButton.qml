import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

Button {
    id: control
    implicitHeight: 34

    foregroundColor: ColorGroup {
        normal: Theme.color.lightorange2
        hover: Theme.color.lightorange1
        down: Theme.color.darkorange1
        disabled: Theme.color.mediumorange1
    }

    backgroundColor: ColorGroup {
        normal: Theme.color.transparent
        hover: Theme.color.mediumorange2
        down: Theme.color.lightorange2
        disabled: Theme.color.darkorange2
    }

    strokeColor: ColorGroup {
        normal: Theme.color.mediumorange1
        hover: Theme.color.lightorange1
        down: Theme.color.lightorange2
        disabled: Theme.color.darkorange2
    }
}
