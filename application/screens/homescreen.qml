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
    signal prefsRequested()

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
                const channelName = preferences.updateChannel;
                const latestVersion = firmwareUpdates.latestVersion;

                let messageObj, actionFunc;

                if(device.state.isRecoveryMode) {
                    messageObj = {
                        title : qsTr("Repair device with version %1?").arg(latestVersion.number),
                        subtitle : qsTr("WARNING! This will fully erase the contents of internal storage.")
                    };

                    actionFunc = function() {
                        device.updater.fullRepair(latestVersion);
                    }

                } else {
                    messageObj = {
                        title : qsTr("Install version %1?").arg(latestVersion.number),
                        subtitle : qsTr("This will install the latest available %1 version.").arg(channelName.toUpperCase())
                    };

                    actionFunc = function() {
                        device.updater.fullUpdate(latestVersion);
                    }
                }

                confirmationDialog.openWithMessage(actionFunc, messageObj);
            }

            onVersionListRequested: {
                screen.versionsRequested(device);
            }

            onScreenStreamRequested: {
                screen.streamRequested(device);
            }

            onLocalUpdateRequested: {
                const messageObj = {
                    title : qsTr("Install update from a file?"),
                    subtitle : qsTr("Caution: this may brick your device.<br/>User settings will NOT be saved.")
                };

                fileDialog.openWithConfirmation(["Firmware files (*.dfu)", "All files (*)"], function() {
                    device.updater.localFirmwareUpdate(fileDialog.fileUrl);
                }, messageObj);
            }

            onLocalRadioUpdateRequested: {
                const messageObj = {
                    title : qsTr("Update the wireless stack?"),
                    subtitle : qsTr("Warning: this operation may need several attempts.<br/>User settings will NOT be saved.")
                };

                fileDialog.openWithConfirmation(["Wireless firmware files (*.bin)", "All files (*)"], function() {
                    device.updater.localWirelessStackUpdate(fileDialog.fileUrl);
                }, messageObj);
            }

            onLocalFUSUpdateRequested: {
                const messageObj = {
                    title : qsTr("Update the FUS?"),
                    subtitle : qsTr("WARNING: this operation will ERASE your SECURITY KEYS.<br/> You MUST know what you are doing.")
                };

                fileDialog.openWithConfirmation(["FUS firmware files (*.bin)", "All files (*)"], function() {
                    device.updater.localFUSUpdate(fileDialog.fileUrl);
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

    StyledToolButton {
        id: prefsButton
        anchors.right: screen.right
        anchors.bottom: screen.bottom
        anchors.margins: 6

        color: "darkgray"
        icon: "qrc:/assets/symbol-gear.svg"

        onPressed: screen.prefsRequested()
    }
}
