import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "../components"

Item {
    id: screen

    property var device

    signal homeRequested()

    anchors.fill: parent

    StyledChangelogDialog {
        id: changelogDialog
    }

    ColumnLayout {
        id: screenLayout

        anchors.fill: parent
        anchors.margins: 16

        spacing: 16

        RowLayout{
            spacing: 16

            ComboBox {
                id: channelSelector
                model: updateRegistry.channels

                onActivated: {
                    updateRegistry.channel = channelSelector.textAt(index);
                }
            }

            Text {
                text: updateRegistry.channelDescription
                color: "white"
                font.pointSize: 12
            }
        }

        ListView {
            id: versionList
            model: updateRegistry

            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 6
            clip: true

            delegate: VersionListDelegate {
                onInstallRequested: {
                    downloader.downloadRemoteFile(screen.device, file);
                    screen.homeRequested();
                }

                onChangelogRequested: {
                    changelogDialog.titleText = title;
                    changelogDialog.contentText = text;
                    changelogDialog.open();
                }
            }

            ScrollBar.vertical: ScrollBar {
                id: scrollbar
                active: true
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

        StyledButton {
            text: qsTr("Back")
            Layout.alignment: Qt.AlignRight
            onClicked: screen.homeRequested()
        }
    }


    Component.onCompleted: {
        updateRegistry.target = screen.device.target
        channelSelector.currentIndex = channelSelector.find(updateRegistry.channel, Qt.MatchFixedString);
    }
}
