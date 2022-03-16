import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0
import QFlipper 1.0

CustomDialog {
    id: control

    closable: false
    closePolicy: Popup.NoAutoClose

    property alias value: progressBar.value
    property alias text: progressLabel.text
    property alias indeterminate: progressBar.indeterminate

    contentWidget: Item {
        implicitWidth: 430
        implicitHeight: layout.implicitHeight

        ColumnLayout {
            id: layout
            width: parent.implicitWidth

            ProgressBar {
                id: progressBar

                implicitWidth: 286
                implicitHeight: 56

                from: 0
                to: 100

                Layout.topMargin: 40
                Layout.bottomMargin: 20
                Layout.alignment: Qt.AlignHCenter
            }

            TextLabel {
                id: progressLabel
                padding: 0

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                Layout.fillWidth: true
                Layout.bottomMargin: 14
            }
        }
    }
}
