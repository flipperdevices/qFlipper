import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "../components"

Item {
    id: screen
    anchors.fill: parent

    signal homeRequested

    property var device

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
                implicitWidth: 200
                model: firmwareUpdates.channelNames

                onActivated: {
                    const channel = firmwareUpdates.channel(textAt(index));

                    versionList.model = channel.versions;
                    descriptionLabel.text = channel.description;
                }
            }

            Text {
                id: descriptionLabel
                color: "white"
                font.pixelSize: 16
            }
        }

        ListView {
            id: versionList

            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 6
            clip: true

            delegate: VersionListDelegate {
                onInstallRequested: {
                    screen.homeRequested();
                    downloader.downloadRemoteFile(screen.device, versionInfo);
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
                font.pixelSize: 30
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
        const defaultChannelName = "release";
        const defaultChannelNameIdx = channelSelector.find(defaultChannelName, Qt.MatchFixedString);

        channelSelector.currentIndex = defaultChannelNameIdx;
        channelSelector.activated(defaultChannelNameIdx);
    }
}
