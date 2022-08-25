import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0
import Primitives 1.0

SmallButton {
    id: control

    foregroundColor: ColorSet {
        normal: Theme.color.lightred2
        hover: Theme.color.lightred1
        down: Theme.color.darkred1
        disabled: Theme.color.mediumred1
    }

    backgroundColor: ColorSet {
        normal: Theme.color.transparent
        hover: Theme.color.mediumred2
        down: Theme.color.lightred2
        disabled: Theme.color.transparent
    }

    strokeColor: ColorSet {
        normal: Theme.color.mediumred1
        hover: Theme.color.lightred1
        down: Theme.color.lightred2
        disabled: Theme.color.mediumred1
    }
}
