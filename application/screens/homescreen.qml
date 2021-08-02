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
        id: confirmationDialog

        width: deviceList.width + 2
        height: deviceList.height + 2

        function openWithMessage(onAcceptedFunc, messageObj = {}) {
            const onDialogRejected = function() {
                confirmationDialog.rejected.disconnect(onDialogRejected);
                confirmationDialog.accepted.disconnect(onDialogAccepted);
            }

            const onDialogAccepted = function() {
                onDialogRejected();
                onAcceptedFunc();
            }

            confirmationDialog.title = messageObj.title ? messageObj.title : qsTr("Question");
            confirmationDialog.subtitle = messageObj.subtitle ? messageObj.subtitle : qsTr("Proceed with the operation?");

            confirmationDialog.rejected.connect(onDialogRejected);
            confirmationDialog.accepted.connect(onDialogAccepted);
            confirmationDialog.open();
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose a file")
        folder: shortcuts.home
        nameFilters: ["Firmware files (*.dfu)", "All files (*)"]

        function openWithConfirmation(nameFilters, onAcceptedFunc, messageObj = {}) {
            const onDialogRejected = function() {
                fileDialog.rejected.disconnect(onDialogRejected);
                fileDialog.accepted.disconnect(onDialogAccepted);
            }

            const onDialogAccepted = function() {
                onDialogRejected();
                confirmationDialog.openWithMessage(onAcceptedFunc, messageObj)
            }

            fileDialog.accepted.connect(onDialogAccepted);
            fileDialog.rejected.connect(onDialogRejected);
            fileDialog.setNameFilters(nameFilters);
            fileDialog.open();
        }
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
                const messageObj = {
                    title : qsTr("Update to version ") + updateRegistry.latestVersion(device.target) + "?",
                    subtitle : qsTr("This will install the latest available stable firmware version.")
                };

                confirmationDialog.openWithMessage(function() {
                    downloader.downloadRemoteFile(device, updateRegistry.latestFirmware(device.target));
                }, messageObj);
            }

            onVersionListRequested: {
                screen.versionsRequested(device)
            }

            onScreenStreamRequested: {
                screen.streamRequested(device)
            }

            onLocalUpdateRequested: {
                const messageObj = {
                    title : qsTr("Install update from the file?"),
                    subtitle : qsTr("Caution: this may brick your device.")
                };

                fileDialog.openWithConfirmation(["Firmware files (*.dfu)", "All files (*)"], function() {
                    downloader.downloadLocalFile(device, fileDialog.fileUrl);
                }, messageObj);
            }

            onLocalRadioUpdateRequested: {
                const messageObj = {
                    title : qsTr("Update the wireless stack?"),
                    subtitle : qsTr("Warning: this operation is potetntially unstable<br/>and may need several attempts.")
                };

                fileDialog.openWithConfirmation(["Radio firmware files (*.bin)", "All files (*)"], function() {
                    downloader.downloadLocalWirelessStack(device, fileDialog.fileUrl);
                }, messageObj);
            }

            onLocalFUSUpdateRequested: {
                const messageObj = {
                    title : qsTr("Update the FUS?"),
                    subtitle : qsTr("WARNING: this operation is potentially unstable<br/>and may need several attempts.")
                };

                fileDialog.openWithConfirmation(["FUS firmware files (*.bin)", "All files (*)"], function() {
                    downloader.downloadLocalFUS(device, fileDialog.fileUrl);
                }, messageObj);
            }

            onFixBootRequested: {
                const messageObj = {
                    title : qsTr("Attempt to fix boot issues?"),
                    subtitle : qsTr("This will try to correct device option bytes.")
                };

                confirmationDialog.openWithMessage(function() {
                    downloader.fixBootIssues(device);
                }, messageObj);
            }

            onOptionBytesCheckRequested: {
                const messageObj = {
                    title : qsTr("Check the Option Bytes?"),
                    subtitle : qsTr("Results will be displayed in the program log.")
                };

                fileDialog.openWithConfirmation(["Option bytes description files (*.data)", "All files (*)"], function() {
                    downloader.checkOptionBytes(device, fileDialog.fileUrl);
                }, messageObj);
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
        text: Qt.application.displayName
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
        text: Qt.application.name + " Version " + Qt.application.version
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        color: "#555"
        font.capitalization: Font.AllUppercase
    }
}
