import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

ColumnLayout {
    id: control
    spacing: 10

    TransparentLabel {
        color: Theme.color.lightorange2
        text: qsTr("Firmware update channel")
        font.capitalization: Font.AllUppercase
    }

    ComboBox {
        id: channelComboBox

        model: firmwareUpdates.channelNames
        delegate: ChannelDelegate {}
        Layout.fillWidth: true

        currentIndex: firmwareUpdates.isReady ? find(preferences.updateChannel) : -1
        onActivated: preferences.updateChannel = textAt(index);
    }

    TransparentLabel {
        color: Theme.color.lightorange2
        text: qsTr("Backup & Restore")
        font.capitalization: Font.AllUppercase
    }

    GridLayout {
        columns: 2
        rowSpacing: control.spacing
        columnSpacing: control.spacing

        Layout.fillWidth: true

        SmallButton {
            text: qsTr("Backup")
            Layout.fillWidth: true

            icon.source: "qrc:/assets/gfx/symbolic/backup-symbolic.svg"
            icon.width: 18
            icon.height: 20
        }

        SmallButton {
            text: qsTr("Restore")
            Layout.fillWidth: true

            icon.source: "qrc:/assets/gfx/symbolic/restore-symbolic.svg"
            icon.width: 18
            icon.height: 20
        }

        SmallButtonRed {
            text: qsTr("Erase")
            Layout.fillWidth: true

            icon.source: "qrc:/assets/gfx/symbolic/trashcan.svg"
            icon.width: 18
            icon.height: 20
        }

        SmallButtonRed {
            text: qsTr("Reinstall")
            Layout.fillWidth: true

            icon.source: "qrc:/assets/gfx/symbolic/update-symbolic.svg"
            icon.width: 16
            icon.height: 16
        }
    }

    TransparentLabel {
        color: Theme.color.lightorange2
        text: qsTr("Application update")
        font.capitalization: Font.AllUppercase
    }

    Button {
        text: qsTr("Update qFlipper")
        Layout.fillWidth: true

        icon.source: "qrc:/assets/gfx/symbolic/update-symbolic.svg"
        icon.width: 16
        icon.height: 16
    }
}
