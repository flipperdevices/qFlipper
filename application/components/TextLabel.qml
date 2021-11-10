import QtQuick 2.15
import Theme 1.0

Text {
    property bool capitalized: true

    color: Theme.color.lightorange2
    font.capitalization: capitalized ? Font.AllUppercase : Font.MixedCase
}
