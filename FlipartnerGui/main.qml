import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12

import "./components"
import "./screens"

Window {
    id: root

    width: 800
    height: 480
    visible: true
    title: qsTr("Flipartner")
    color: "black"

    StyledConfirmationDialog {
        id: updateConfirmationDialog

        width: deviceList.width + 2
        height: deviceList.height + 2

        subtitle: qsTr("Fetching updates from the server is not implemented yet")
    }

    StyledConfirmationDialog {
        id: fileConfirmationDialog

        width: deviceList.width + 2
        height: deviceList.height + 2

        title: qsTr("Install update from local file?")
        subtitle: qsTr("Warning: this operation may brick your device")
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file")
        folder: shortcuts.home
        nameFilters: ["Firmware files (*.dfu)", "All files (*)"]
    }

    VersionListDialog {
        id: versionDialog
    }

    ListView {
        id: deviceList
        model: deviceRegistry
        anchors.fill: parent

        anchors.leftMargin: 100
        anchors.rightMargin: 100
        anchors.topMargin: parent.height/4
        anchors.bottomMargin: parent.height/4

        spacing: 6
        clip: true

        delegate: FlipperListDelegate {
            onUpdateRequested: {
                updateConfirmationDialog.title = qsTr("Update to version ") + updateRegistry.latestVersion(device.target) + "?";

                const onUpdateDialogClosed = function() {
                    updateConfirmationDialog.closed.disconnect(onUpdateDialogClosed);
                    updateConfirmationDialog.accepted.disconnect(onUpdateDialogAccepted);
                }

                const onUpdateDialogAccepted = function() {
                    onUpdateDialogClosed();
                    downloader.downloadRemoteFile(device, updateRegistry.latestFirmware(device.target));
                }

                updateConfirmationDialog.closed.connect(onUpdateDialogClosed);
                updateConfirmationDialog.accepted.connect(onUpdateDialogAccepted);

                updateConfirmationDialog.open();
            }

            onVersionListRequested: {
                updateRegistry.target = device.target

                const onVersionDialogClosed = function() {
                    versionDialog.closed.disconnect(onVersionDialogClosed);
                    versionDialog.accepted.disconnect(onVersionDialogAccepted);
                }

                const onVersionDialogAccepted = function() {
                    onVersionDialogClosed();

                    downloader.downloadRemoteFile(device, versionDialog.fileInfo);
                }

                versionDialog.accepted.connect(onVersionDialogAccepted);
                versionDialog.closed.connect(onVersionDialogClosed);
                versionDialog.open();
            }

            onLocalUpdateRequested: {
                const onFileDialogClosed = function() {
                    fileDialog.rejected.disconnect(onFileDialogClosed);
                    fileDialog.accepted.disconnect(onFileDialogAccepted);
                }

                const onFileDialogAccepted = function() {
                    onFileDialogClosed();

                    const onConfirmDialogAccepted = function() {
                        downloader.downloadLocalFile(device, fileDialog.fileUrl);
                    };

                    const onConfirmDialogClosed = function() {
                        fileConfirmationDialog.accepted.disconnect(onConfirmDialogAccepted);
                        fileConfirmationDialog.closed.disconnect(onConfirmDialogClosed);
                    }

                    fileConfirmationDialog.accepted.connect(onConfirmDialogAccepted);
                    fileConfirmationDialog.closed.connect(onConfirmDialogClosed);
                    fileConfirmationDialog.open();
                };

                fileDialog.accepted.connect(onFileDialogAccepted);
                fileDialog.rejected.connect(onFileDialogClosed);
                fileDialog.open();
            }
        }

        Text {
            id: noDevicesLabel
            text: qsTr("No devices connected")
            anchors.centerIn: parent
            color: "#444"
            font.pointSize: 24
            visible: deviceList.count === 0
        }
    }

    Text {
        id: titleLabel
        anchors.bottom: deviceList.top
        anchors.bottomMargin: 40
        anchors.horizontalCenter: deviceList.horizontalCenter
        text: "Flipartner"
        font.pointSize: 24
        font.capitalization: Font.AllUppercase
        color: "white"
    }

    Text {
        id: subtitleLabel
        anchors.top: titleLabel.bottom
        anchors.horizontalCenter: titleLabel.horizontalCenter
        text: "a Flipper companion app"
        color: "darkgray"
        font.capitalization: Font.AllUppercase
    }

    Text {
        text: "a cool bottom line"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        color: "#333"
        font.capitalization: Font.AllUppercase
    }
}
