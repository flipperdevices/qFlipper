import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

ColumnLayout {
    id: control
    spacing: 10

    TransparentLabel {
        height: 14
        color: Theme.color.orange
        text: qsTr("Header one")
        font.capitalization: Font.AllUppercase
    }

//    ComboBox {
//        model: ["One", "Two", "Three"]
//    }

    Button {
        text: "Hello there!"
    }

    Button {
        text: "General Kenobi!"
    }

    TransparentLabel {
        height: 14
        color: Theme.color.orange
        text: qsTr("Header two a longer one")
        font.capitalization: Font.AllUppercase
    }

    Button {
        text: "So uncivilised"
    }
}
