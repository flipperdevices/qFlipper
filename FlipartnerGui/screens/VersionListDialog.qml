import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "../components"

Dialog {
    id: screen

    width: parent.width
    height: parent.height

    property var fileInfo

    StyledChangelogDialog {
        id: changelogDialog
    }

    background: Rectangle {
        id: bg
        color: "black"
    }

    header: RowLayout{
        spacing: 16

        ComboBox {
            id: channelSelector
            model: updateRegistry.channels

            onActivated: {
                updateRegistry.channel = channelSelector.textAt(index);
            }

            Layout.topMargin: parent.spacing
            Layout.leftMargin: parent.spacing
        }

        Text {
            text: updateRegistry.channelDescription
            color: "white"
            font.pointSize: 12

            Layout.fillWidth: true
            Layout.topMargin: parent.spacing
        }
    }

    contentItem: ListView {
        id: versionList
        model: updateRegistry

        spacing: 6
        clip: true

        delegate: VersionListDelegate {
            onInstallRequested: {
                screen.fileInfo = file;
                screen.accept();
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
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        onClicked: screen.close();
    }

    onOpened: {
        channelSelector.currentIndex = channelSelector.find(updateRegistry.channel, Qt.MatchFixedString);
    }
}
