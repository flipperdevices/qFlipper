import QtQuick 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: control
    spacing: 30

    Item {
        Layout.fillWidth: true
    }

    ColumnLayout {
        id: keys

        TextLabel {
            text: qsTr("Version")
            Layout.alignment: Qt.AlignRight
        }

        TextLabel {
            text: qsTr("Date")
            Layout.alignment: Qt.AlignRight
        }

        TextLabel {
            text: qsTr("Hardware")
            Layout.alignment: Qt.AlignRight
        }

        TextLabel {
            text: qsTr("Battery")
            Layout.alignment: Qt.AlignRight
        }
    }

    ColumnLayout {
        id: values

        TextLabel {
            text: "0.42.0"
        }

        TextLabel {
            text: "2021-09-15"
        }

        TextLabel {
            text: "11.F7.B8.C3"
        }

        TextLabel {
            text: "69%"
        }
    }

    Item {
        Layout.fillWidth: true
    }
}
