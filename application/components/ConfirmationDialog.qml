import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: dialog

    contentItem: Item {

        Rectangle {
            implicitWidth: 300
            implicitHeight: 200
            anchors.centerIn: parent

            radius: 7
            border.width: 2

            color: "black"
            border.color: Theme.color.orange
        }
    }
}
