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

            opacity: 0.5
            color: "black"

            radius: background.radius - anchors.margins
        }

        Rectangle {
            id: background

            anchors.fill: parent
            anchors.topMargin: 60
            anchors.leftMargin: 35
            anchors.rightMargin: 35
            anchors.bottomMargin: 60

            radius: 8
            border.width: 2

            color: "black"
            border.color: Theme.color.lightorange2

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    radius: background.radius - background.border.width
                    color: Theme.color.darkorange1

                    Layout.fillWidth: true
                    Layout.preferredHeight: 42
                    Layout.margins: background.border.width
                    Layout.bottomMargin: 0

                    Rectangle {
                        color: parent.color
                        width: parent.width
                        height: parent.radius
                        anchors.bottom: parent.bottom
                    }

                    RowLayout {
                        anchors.fill: parent

                        TextLabel {
                            font.bold: true
                            text: firmwareUpdates.isReady ? qsTr("Version %1 changelog").arg(firmwareUpdates.latestVersion.number) : qsTr("No data")

                            Layout.fillWidth: true

                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.preferredWidth: height - 6

                            WindowButton {
                                anchors.centerIn: parent
                                iconName: "close"
                                iconPath: "qrc:/assets/gfx/controls/windows"

                                Layout.alignment: Qt.AlignVCenter

                                onClicked: control.close()
                            }
                        }
                    }
                }

                ScrollView {
                    id: scrollView
                    clip: true

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    background.visible: false
                    contentWidth: availableWidth

                    Text {
                        width: scrollView.availableWidth
                        text: firmwareUpdates.isReady ? firmwareUpdates.latestVersion.changelog : qsTr("No data")
                        textFormat: Text.MarkdownText
                        color: Theme.color.lightorange2
                        wrapMode: Text.Wrap
                    }
                }
            }
        }
    }
}
