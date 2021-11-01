import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

RowLayout {
    id: control

    property int padding

    property alias icon: image
    property alias text: label.text
    property alias font: label.font
    property alias color: label.color

    IconImage {
        id: image
        Layout.leftMargin: control.padding
    }

    Text {
        id: label

        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter

        Layout.fillWidth: true
        Layout.rightMargin: control.padding
    }
}
