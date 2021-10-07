import QtQuick 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12

import QFlipper 1.0

import "../components"

Item {
    id: screen
    anchors.fill: parent

    signal versionsRequested(var device)
    signal streamRequested(var device)

    readonly property string channelName: "development" //TODO move this property into application settings
    readonly property bool hasUpdates: {
        const currentVersion = app.version;
        const currentCommit = app.commit;

        if(applicationUpdates.channelNames.length === 0) {
            return false;
        }

        const latestVersion = applicationUpdates.channel(channelName).latestVersion;

        const newDevVersionAvailable = (channelName === "development") && (latestVersion.number !== currentCommit);
        const newReleaseVersionAvailable = (channelName !== "development") && (latestVersion.number > currentVersion);

        return newDevVersionAvailable || newReleaseVersionAvailable;
    }

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

    FileDialog {
        id: dirDialog
        title: qsTr("Please choose a directory")
        folder: shortcuts.home
        selectFolder: true

        function openWithConfirmation(onAcceptedFunc, messageObj = {}) {
            const onDialogRejected = function() {
                dirDialog.rejected.disconnect(onDialogRejected);
                dirDialog.accepted.disconnect(onDialogAccepted);
            }

            const onDialogAccepted = function() {
                onDialogRejected();
                confirmationDialog.openWithMessage(onAcceptedFunc, messageObj)
            }

            dirDialog.accepted.connect(onDialogAccepted);
            dirDialog.rejected.connect(onDialogRejected);
            dirDialog.open();
        }
    }

    ListView {
        id: deviceList
        model: deviceRegistry
        anchors.fill: parent

        anchors.leftMargin: 100
        anchors.rightMargin: 100
        anchors.topMargin: parent.height/4
        anchors.bottomMargin: 50

        spacing: 6
        clip: true

        delegate: FlipperListDelegate {
            onUpdateRequested: {
                const channelName = "release";
                const latestVersion = "Hello";//firmwareUpdates.channel(channelName).latestVersion;

                const messageObj = {
                    title : qsTr("Install version %1?").arg(latestVersion.number),
                    subtitle : qsTr("This will install the latest available %1 version.").arg(channelName.toUpperCase())
                };

                confirmationDialog.openWithMessage(function() {
                    device.updater.fullUpdate();
                }, messageObj);
            }

            onVersionListRequested: {
                screen.versionsRequested(device);
            }

            onScreenStreamRequested: {
                screen.streamRequested(device);
            }

            onLocalUpdateRequested: {
                const messageObj = {
                    title : qsTr("Install update from the file?"),
                    subtitle : qsTr("Caution: this may brick your device.")
                };

                fileDialog.openWithConfirmation(["Firmware files (*.dfu)", "All files (*)"], function() {
//                    downloader.downloadLocalFile(device, fileDialog.fileUrl);
                }, messageObj);
            }

            onBackupRequested: {
                const messageObj = {
                    title : qsTr("Backup user data?"),
                    subtitle : qsTr("This will backup the contents of internal storage.")
                };

                dirDialog.openWithConfirmation(function() {
//                    downloader.backupUserData(device, dirDialog.fileUrl);
                }, messageObj);
            }

            onRestoreRequested: {
                const messageObj = {
                    title : qsTr("Restore user data?"),
                    subtitle : qsTr("This will restore the contents of internal storage.")
                };

                dirDialog.openWithConfirmation(function() {
//                    downloader.restoreUserData(device, dirDialog.fileUrl);
                }, messageObj);
            }

            onLocalAssetsUpdateRequested: {
                const messageObj = {
                    title : qsTr("Update the databases?"),
                    subtitle : qsTr("This will install the databases from a file.")
                };

                fileDialog.openWithConfirmation(["Database files (*.tgz)", "All files (*)"], function() {
//                    downloader.downloadAssets(device, fileDialog.fileUrl);
                }, messageObj);
            }

            onLocalRadioUpdateRequested: {
                const messageObj = {
                    title : qsTr("Update the wireless stack?"),
                    subtitle : qsTr("Warning: this operation is potetntially unstable<br/>and may need several attempts.")
                };

                fileDialog.openWithConfirmation(["Radio firmware files (*.bin)", "All files (*)"], function() {
//                    downloader.downloadLocalWirelessStack(device, fileDialog.fileUrl);
                }, messageObj);
            }

            onLocalFUSUpdateRequested: {
                const messageObj = {
                    title : qsTr("Update the FUS?"),
                    subtitle : qsTr("WARNING: this operation is potentially unstable<br/>and may need several attempts.")
                };

                fileDialog.openWithConfirmation(["FUS firmware files (*.bin)", "All files (*)"], function() {
//                    downloader.downloadLocalFUS(device, fileDialog.fileUrl);
                }, messageObj);
            }

            onFixBootRequested: {
                const messageObj = {
                    title : qsTr("Attempt to fix boot issues?"),
//                    subtitle : qsTr("This will try to correct device option bytes.")
                };

                confirmationDialog.openWithMessage(function() {
//                    downloader.fixBootIssues(device);
                }, messageObj);
            }

            onFixOptionBytesRequested: {
                const messageObj = {
                    title : qsTr("Check and fix the Option Bytes?"),
                    subtitle : qsTr("Results will be displayed in the program log.")
                };

                fileDialog.openWithConfirmation(["Option bytes description files (*.data)", "All files (*)"], function() {
//                    downloader.fixOptionBytes(device, fileDialog.fileUrl);
                }, messageObj);
            }
        }
    }

    Text {
        id: titleLabel
        anchors.bottom: deviceList.top
        anchors.bottomMargin: 40
        anchors.horizontalCenter: deviceList.horizontalCenter
        text: Qt.application.displayName
        font.pixelSize: 30
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
        id: noDevicesLabel
        text: qsTr("No devices connected")
        anchors.centerIn: parent
        color: "#444"
        font.pixelSize: 30
        visible: deviceList.count === 0
    }

    Text {
        id: versionLabel

        text: {
            if(app.updater.state === AppUpdater.Idle) {
                const msg = "%1 %2 %3".arg(app.name).arg(qsTr("Version")).arg(app.version);

                if(screen.hasUpdates) {
                    return "%1 - %2".arg(msg).arg(qsTr("<a href=\"#\">update available!</a>"));
                } else {
                    return msg;
                }

            } else if(app.updater.state === AppUpdater.Downloading) {
                return qsTr("Downloading application update... %1%").arg(Math.floor(app.updater.progress));
            } else if(app.updater.state === AppUpdater.Updating) {
                return qsTr("Starting the update process...");
            } else if(app.updater.state === AppUpdater.ErrorOccured) {
                return qsTr("Update failed. <a href=\"#\">Retry?</a>");
            }
        }

        color: {
            if(app.updater.state === AppUpdater.Downloading) {
                return "darkgray";
            } else if(app.updater.state === AppUpdater.Updating) {
                return "darkgray";
            } else if(app.updater.state === AppUpdater.ErrorOccured) {
                return "#F55";
            } else if(screen.hasUpdates) {
                return "darkgray";
            } else {
                return "#555";
            }
        }

        linkColor: "#5eba7d"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        font.capitalization: Font.AllUppercase
        textFormat: Text.StyledText

        onLinkActivated: {
            app.updater.installUpdate(applicationUpdates.channel(channelName).latestVersion);
        }
    }
}
