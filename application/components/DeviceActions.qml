import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0
import QFlipper 1.0

ColumnLayout {
    id: control
    spacing: 10

    property alias backupAction: backupAction
    property alias restoreAction: restoreAction
    property alias eraseAction: eraseAction
    property alias reinstallAction: reinstallAction
    property alias selfUpdateAction: selfUpdateAction

    readonly property var device: Backend.currentDevice
    readonly property var deviceState: device ? device.state : undefined

    TransparentLabel {
        color: Theme.color.lightorange2
        text: qsTr("Firmware update channel")
    }

    ComboBox {
        id: channelComboBox

        model: firmwareUpdates.channelNames

        delegate: ChannelDelegate {
            objectName: index === channelComboBox.model.length - 1 ? "last" : ""
        }

        Layout.fillWidth: true

        currentIndex: firmwareUpdates.isReady ? find(Preferences.updateChannel) : -1
        onActivated: Preferences.updateChannel = textAt(index);
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
    }

    Button {
        action: selfUpdateAction
        Layout.fillWidth: true

        icon.source: "qrc:/assets/gfx/symbolic/update-symbolic.svg"
        icon.width: 16
        icon.height: 16
    }

    Action {
        id: backupAction
        text: qsTr("Backup")
        enabled: !!deviceState && !deviceState.isRecoveryMode
    }

    Action {
        id: restoreAction
        text: qsTr("Restore")
        enabled: !!deviceState && !deviceState.isRecoveryMode
    }

    Action {
        id: eraseAction
        text: qsTr("Erase")
        enabled: !!deviceState && !deviceState.isRecoveryMode
    }

    Action {
        id: reinstallAction
        text: qsTr("Reinstall")
        enabled: firmwareUpdates.isReady && !!deviceState && !deviceState.isRecoveryMode &&
                 !(device.updater.canUpdate(firmwareUpdates.latestVersion) || device.updater.canInstall())
    }

    Action {
        id: selfUpdateAction
        text: qsTr("Update qFlipper")
        enabled: applicationUpdates.isReady && app.updater.canUpdate(applicationUpdates.latestVersion)
    }
}
