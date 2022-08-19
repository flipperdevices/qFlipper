import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../styles/DefaultAmber"

Button {
    id: control
    flat: true
    padding: 0
    font.capitalization: Font.MixedCase

    property color linkColor: Theme.color.lightorange2

    foregroundColor: ColorGroup {
        normal: linkColor
        hover: Qt.lighter(linkColor, 1.2)
        down: linkColor
        disabled: Theme.color.mediumorange1
    }

    backgroundColor: ColorGroup {
        normal: Theme.color.transparent
        hover: Theme.color.transparent
        down: Theme.color.transparent
        disabled: Theme.color.transparent
    }
}
