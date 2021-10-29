import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

SmallButton {
    id: control

    foregroundColor: ColorGroup {
        normal: Theme.color.lightred2
        hover: Theme.color.lightred1
        down: Theme.color.darkred
        disabled: Theme.color.mediumred1
    }

    backgroundColor: ColorGroup {
        normal: Theme.color.transparent
        hover: Theme.color.mediumred2
        down: Theme.color.lightred2
        disabled: Theme.color.transparent
    }

    strokeColor: ColorGroup {
        normal: Theme.color.mediumred1
        hover: Theme.color.lightred1
        down: Theme.color.lightred2
        disabled: Theme.color.mediumred1
    }
}
