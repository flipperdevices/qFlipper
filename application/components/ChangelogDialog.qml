import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

    contentItem: Item {
        Rectangle {
            anchors.fill: background
            anchors.margins: -3
            color: Theme.color.mediumorange3
            radius: background.radius - anchors.margins
        }

        Rectangle {
            id: background
            implicitWidth: 570
            implicitHeight: 300
            anchors.centerIn: parent

            radius: 7
            border.width: 2

            color: "black"
            border.color: Theme.color.lightorange2

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    Layout.margins: 6

                    TextLabel {
                        leftPadding: 10
                        Layout.fillWidth: true
                        text: firmwareUpdates.isReady ? qsTr("Version %1 changelog:").arg(firmwareUpdates.latestVersion.number) : qsTr("No data")
                    }

                    WindowButton {
                        iconName: "close"
                        iconPath: "qrc:/assets/gfx/controls/windows"

                        Layout.alignment: Qt.AlignVCenter

                        onClicked: control.close()
                    }
                }

                ScrollView {
                    clip: true

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    background.visible: false

                    Text {
                        text: firmwareUpdates.isReady ? firmwareUpdates.latestVersion.changelog : qsTr("No data")
                        textFormat: Text.MarkdownText
                        color: Theme.color.lightorange2
                    }
                }
            }
        }
    }
}
