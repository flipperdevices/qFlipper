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

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined

    TransparentLabel {
        color: Theme.color.lightorange2
        text: qsTr("Firmware update channel")
    }

    ComboBox {
        id: channelComboBox

        model: firmwareUpdates.channelNames
        delegate: ChannelDelegate {}
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
        }

        SmallButton {
            action: restoreAction
            Layout.fillWidth: true

            icon.source: "qrc:/assets/gfx/symbolic/restore-symbolic.svg"
            icon.width: 18
            icon.height: 20
        }

        SmallButtonRed {
            action: eraseAction
            Layout.fillWidth: true

            icon.source: "qrc:/assets/gfx/symbolic/trashcan.svg"
            icon.width: 18
            icon.height: 20
        }

        SmallButton {
            action: reinstallAction
            Layout.fillWidth: true

            icon.source: "qrc:/assets/gfx/symbolic/update-symbolic.svg"
            icon.width: 16
            icon.height: 16
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
