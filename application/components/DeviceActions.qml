import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

ColumnLayout {
    id: control
    spacing: 10

    TransparentLabel {
        height: 14
        color: Theme.color.orange
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
        height: 14
        color: Theme.color.orange
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
        }

        SmallButton {
            text: qsTr("Restore")
            Layout.fillWidth: true
            icon.source: "qrc:/assets/gfx/symbolic/restore-symbolic.svg"
        }

        SmallButtonRed {
            text: qsTr("Erase")
            Layout.fillWidth: true
            icon.source: "qrc:/assets/gfx/symbolic/trashcan.svg"
        }
    }

    TransparentLabel {
        height: 14
        color: Theme.color.orange
        text: qsTr("Application update")
        font.capitalization: Font.AllUppercase
    }

    Button {
        text: qsTr("Update qFlipper")
        Layout.fillWidth: true
        icon.source: "qrc:/assets/gfx/symbolic/update-symbolic.svg"
    }
}
