import QtQuick 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12

import "../components"

Item {
    id: screen
    anchors.fill: parent

    signal versionsRequested(var device)
    signal streamRequested(var device)

    StyledConfirmationDialog {
        id: updateConfirmationDialog

        width: deviceList.width + 2
        height: deviceList.height + 2

        subtitle: qsTr("This will install the latest firmware on your device.")
    }

    StyledConfirmationDialog {
        id: fileConfirmationDialog

        width: deviceList.width + 2
        height: deviceList.height + 2

        title: qsTr("Install update from local file?")
        subtitle: qsTr("Warning: this operation may brick your device.")
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file")
        folder: shortcuts.home
        nameFilters: ["Firmware files (*.dfu)", "All files (*)"]
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

                const onUpdateDialogRejected = function() {
                    updateConfirmationDialog.rejected.disconnect(onUpdateDialogRejected);
                    updateConfirmationDialog.accepted.disconnect(onUpdateDialogAccepted);
                }

                const onUpdateDialogAccepted = function() {
                    onUpdateDialogRejected();
                    downloader.downloadRemoteFile(device, updateRegistry.latestFirmware(device.target));
                }

                updateConfirmationDialog.rejected.connect(onUpdateDialogRejected);
                updateConfirmationDialog.accepted.connect(onUpdateDialogAccepted);

                updateConfirmationDialog.open();
            }

            onVersionListRequested: {
                screen.versionsRequested(device)
            }

            onScreenStreamRequested: {
                screen.streamRequested(device)
            }

            onLocalUpdateRequested: {
                const onFileDialogRejected = function() {
                    fileDialog.rejected.disconnect(onFileDialogRejected);
                    fileDialog.accepted.disconnect(onFileDialogAccepted);
                }

                const onFileDialogAccepted = function() {
                    onFileDialogRejected();

                    const onConfirmDialogAccepted = function() {
                        downloader.downloadLocalFile(device, fileDialog.fileUrl);
                    };

                    const onConfirmDialogRejected = function() {
                        fileConfirmationDialog.accepted.disconnect(onConfirmDialogAccepted);
                        fileConfirmationDialog.rejected.disconnect(onConfirmDialogRejected);
                    }

                    fileConfirmationDialog.accepted.connect(onConfirmDialogAccepted);
                    fileConfirmationDialog.rejected.connect(onConfirmDialogRejected);
                    fileConfirmationDialog.open();
                };

                fileDialog.accepted.connect(onFileDialogAccepted);
                fileDialog.rejected.connect(onFileDialogRejected);
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
