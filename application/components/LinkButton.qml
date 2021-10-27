import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

Button {
    id: control
    flat: true
    font.capitalization: Font.MixedCase

    property color linkColor: Theme.color.orange

    foregroundColor: ColorGroup {
        normal: linkColor
        hover: Qt.lighter(linkColor, 1.2)
        down: linkColor
    }
}
