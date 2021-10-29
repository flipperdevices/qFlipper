import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

Button {
    id: control
    implicitHeight: 34

    foregroundColor: ColorGroup {
        normal: Theme.color.orange
        hover: Theme.color.lightorange
        down: Theme.color.darkorange1
        disabled: Theme.color.mediumorange1
    }

    backgroundColor: ColorGroup {
        normal: Theme.color.transparent
        hover: Theme.color.mediumorange2
        down: Theme.color.orange
        disabled: Theme.color.darkorange2
    }

    strokeColor: ColorGroup {
        normal: Theme.color.mediumorange1
        hover: Theme.color.lightorange
        down: Theme.color.orange
        disabled: Theme.color.darkorange2
    }
}
