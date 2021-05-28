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
            model: firmwareUpdates.channels

            onActivated: {
                firmwareUpdates.channel = channelSelector.textAt(index);
            }
        }

        ListView {
            id: versionList
            model: firmwareUpdates

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
        }
    }

    onOpened: {
        channelSelector.currentIndex = channelSelector.find(firmwareUpdates.channel, Qt.MatchFixedString);
    }
}
