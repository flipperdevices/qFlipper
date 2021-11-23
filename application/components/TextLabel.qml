import QtQuick 2.15
import Theme 1.0

Text {
    property bool capitalized: true

    antialiasing: false

    color: Theme.color.lightorange2

    font.pixelSize: 32
    font.family: "HaxrCorp 4089"
    font.capitalization: capitalized ? Font.AllUppercase : Font.MixedCase
}
