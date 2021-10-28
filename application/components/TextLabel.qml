import QtQuick 2.15
import Theme 1.0

Text {
    property bool capitalized: true

    color: Theme.color.orange
    font.capitalization: capitalized ? Font.AllUppercase : Font.MixedCase
}
