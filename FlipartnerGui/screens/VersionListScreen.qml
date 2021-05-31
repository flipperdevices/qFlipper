import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "../components"

Popup {
    id: screen

    width: parent.width
    height: parent.height

    background: Rectangle {
        id: bg
        color: "black"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 20

        ComboBox {
            id: channelSelector
            model: updateRegistry.channels

            onActivated: {
                updateRegistry.channel = channelSelector.textAt(index);
            }
        }

        ListView {
            id: versionList
            model: updateRegistry

            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 6
            clip: true

            rightMargin: scrollbar.width + spacing

            delegate: VersionListDelegate {
                onInstallRequested: {
                    console.log(file)
                }
            }

            ScrollBar.vertical: ScrollBar {
                id: scrollbar
                active: true
                policy: ScrollBar.AlwaysOn
            }

            Text {
                id: noUpdatesLabel
                text: qsTr("Updates not found")
                anchors.centerIn: parent
                color: "#444"
                font.pointSize: 24
                visible: versionList.count === 0
            }
        }
    }

    onOpened: {
        channelSelector.currentIndex = channelSelector.find(updateRegistry.channel, Qt.MatchFixedString);
    }
}
