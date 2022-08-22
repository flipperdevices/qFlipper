import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0
import Primitives 1.0

Button {
    id: control
    implicitHeight: 34

    foregroundColor: ColorSet {
        normal: Theme.color.lightorange2
        hover: Theme.color.lightorange1
        down: Theme.color.darkorange1
        disabled: Theme.color.mediumorange1
    }

    backgroundColor: ColorSet {
        normal: control.highlighted ? Theme.color.darkorange1 : Theme.color.transparent
        hover: Theme.color.mediumorange2
        down: Theme.color.lightorange2
        disabled: Theme.color.transparent
    }

    strokeColor: ColorSet {
        normal: control.highlighted ? Theme.color.lightorange2 : Theme.color.mediumorange1
        hover: Theme.color.lightorange1
        down: Theme.color.lightorange2
        disabled: Theme.color.mediumorange1
    }
}
