import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0
import QFlipper 1.0


Item {
    id: container

    property alias backupAction: backupAction
    property alias restoreAction: restoreAction
    property alias eraseAction: eraseAction
    property alias reinstallAction: reinstallAction
    property alias selfUpdateAction: selfUpdateAction

    implicitWidth: 318
    implicitHeight: control.implicitHeight + verticalPadding * 2

    readonly property int horizontalPadding: Math.floor((container.implicitWidth - control.implicitWidth) / 2)
    readonly property int verticalPadding: 10

    ColumnLayout {
        id: control
        spacing: 10

        x: horizontalPadding
        y: verticalPadding

        TransparentLabel {
            color: Theme.color.lightorange2
            text: qsTr("Firmware update channel")
        }

        ComboBox {
            id: channelComboBox

            enabled: Backend.firmwareUpdateState !== Backend.Unknown &&
                     Backend.firmwareUpdateState !== Backend.Checking &&
                     Backend.firmwareUpdateState !== Backend.ErrorOccured

            delegate: ChannelDelegate {}

            model: Backend.firmwareUpdateModel
            textRole: "name"

            Layout.fillWidth: true

            currentIndex: Backend.firmwareUpdateState !== Backend.Unknown ? find(Preferences.updateChannel) : -1
            onActivated: function(index) {Preferences.updateChannel = textAt(index);}

            ToolTip {
                visible: parent.hovered
                text: qsTr("Change the firmware update channel")
                implicitWidth: 250
            }
        }

        TransparentLabel {
            color: Theme.color.lightorange2
            text: qsTr("Backup & Restore")
        }

        GridLayout {
            columns: 2
            rowSpacing: control.spacing
            columnSpacing: control.spacing

            Layout.fillWidth: true

            SmallButton {
                action: backupAction
                Layout.fillWidth: true

                icon.source: "qrc:/assets/gfx/symbolic/backup-symbolic.svg"
                icon.width: 18
                icon.height: 20

                ToolTip {
                    visible: parent.hovered
                    text: qsTr("Save the contents of Flipper's internal storage to this computer's disk.")
                    implicitWidth: 250
                }
            }

            SmallButton {
                action: restoreAction
                Layout.fillWidth: true

                icon.source: "qrc:/assets/gfx/symbolic/restore-symbolic.svg"
                icon.width: 18
                icon.height: 20

                ToolTip {
                    visible: parent.hovered
                    text: qsTr("Download the contents of a backup directory to Flipper's internal storage.")
                    implicitWidth: 250
                }
            }

            SmallButtonRed {
                action: eraseAction
                Layout.fillWidth: true

                icon.source: "qrc:/assets/gfx/symbolic/trashcan.svg"
                icon.width: 18
                icon.height: 20

                ToolTip {
                    visible: parent.hovered
                    text: qsTr("Revert Flipper to its default settings. WARNING! All progress will be lost!")
                    implicitWidth: 250
                }
            }

            SmallButton {
                action: reinstallAction
                Layout.fillWidth: true

                icon.source: "qrc:/assets/gfx/symbolic/update-symbolic.svg"
                icon.width: 16
                icon.height: 16

                ToolTip {
                    visible: parent.hovered
                    text: qsTr("Install the current firmware version again. Not for everyday use.")
                    implicitWidth: 250
                }
            }
        }

        TransparentLabel {
            color: Theme.color.lightorange2
            text: qsTr("Application update")
            visible: Preferences.checkAppUpdates
        }

        Button {
            action: selfUpdateAction
            Layout.fillWidth: true
            visible: Preferences.checkAppUpdates

            icon.source: "qrc:/assets/gfx/symbolic/update-symbolic.svg"
            icon.width: 16
            icon.height: 16
        }

        Action {
            id: backupAction
            text: qsTr("Backup")
            enabled: Backend.deviceState && !Backend.deviceState.isRecoveryMode
        }

        Action {
            id: restoreAction
            text: qsTr("Restore")
            enabled: Backend.deviceState && !Backend.deviceState.isRecoveryMode
        }

        Action {
            id: eraseAction
            text: qsTr("Erase")
            enabled: Backend.deviceState && !Backend.deviceState.isRecoveryMode
        }

        Action {
            id: reinstallAction
            text: qsTr("Reinstall")
            enabled: Backend.firmwareUpdateState === Backend.NoUpdates
        }

        Action {
            id: selfUpdateAction
            text: App.updateStatus === App.Checking ? qsTr("Checking...") :
                  App.updateStatus === App.NoUpdates && checkTimer.running ? qsTr("No updates") : qsTr("Check app updates")

            enabled: Preferences.checkAppUpdates && App.updateStatus !== App.Checking && !checkTimer.running
            onTriggered: App.checkForUpdates()
        }

        Timer {
            id: checkTimer
            interval: 1000

            Component.onCompleted: {
                App.updateStatusChanged.connect(function() {
                    if(App.updateStatus === App.NoUpdates) {
                        start();
                    }
                });
            }
        }
    }
}
