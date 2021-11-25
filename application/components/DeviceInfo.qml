import QtQuick 2.15
import QtQuick.Layouts 1.15

import Theme 1.0

RowLayout {
    id: control
    spacing: 30

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined
    readonly property bool extraFields: deviceState ? !deviceState.isRecoveryMode : false

    ColumnLayout {
        id: keys

        TextLabel {
            text: qsTr("Firmware")
            visible: extraFields
            horizontalAlignment: Text.AlignRight
            Layout.fillWidth: true
        }

        TextLabel {
            text: qsTr("Build Date")
            visible: extraFields
            color: Theme.color.mediumorange4
            horizontalAlignment: Text.AlignRight
            Layout.fillWidth: true
        }

        TextLabel {
            text: qsTr("SD Card")
            visible: extraFields
            horizontalAlignment: Text.AlignRight
            color: Theme.color.mediumorange4
            Layout.fillWidth: true
        }

        TextLabel {
            text: qsTr("Databases")
            visible: extraFields
            horizontalAlignment: Text.AlignRight
            color: Theme.color.mediumorange4
            Layout.fillWidth: true
        }

        TextLabel {
            color: extraFields ? Theme.color.mediumorange4 : Theme.color.lightorange2
            text: qsTr("Hardware")
            horizontalAlignment: Text.AlignRight
            Layout.fillWidth: true
        }

        TextLabel {
            text: qsTr("Radio FW")
            visible: extraFields
            horizontalAlignment: Text.AlignRight
            color: Theme.color.mediumorange4
            Layout.fillWidth: true
        }
    }

    ColumnLayout {
        id: values

        TextLabel {
            text: !deviceInfo ? text : deviceInfo.firmware.branch === "dev" ?
                   deviceInfo.firmware.commit : deviceInfo.firmware.version

            visible: extraFields
        }

        TextLabel {
            text: deviceInfo ? deviceInfo.firmware.date.toLocaleDateString(Qt.locale("C"), Locale.ShortFormat) : text
            color: Theme.color.lightorange3
            visible: extraFields
        }

        TextLabel {
            text: deviceInfo && deviceInfo.storage.isExternalPresent ? deviceInfo.storage.externalFree + qsTr("% Free") : qsTr("Not present")
            color: deviceInfo && deviceInfo.storage.isExternalPresent ? Theme.color.lightorange3 : Theme.color.lightred3
            visible: extraFields
        }

        TextLabel {
            text: deviceInfo && deviceInfo.storage.isAssetsInstalled ? qsTr("Installed") : qsTr("Missing")
            color: deviceInfo && deviceInfo.storage.isAssetsInstalled ? Theme.color.lightorange3 : Theme.color.lightred3
            visible: extraFields
        }

        TextLabel {
            color: extraFields ? Theme.color.lightorange3 : Theme.color.lightorange2

            text: {
                if(!deviceInfo) {
                    return text;
                } else {
                    return [
                        deviceInfo.hardware.version,
                        deviceInfo.hardware.target,
                        deviceInfo.hardware.body,
                        deviceInfo.hardware.connect
                    ].join(".")
                }
            }

            Layout.fillWidth: true
        }

        TextLabel {
            text: deviceInfo ? deviceInfo.radioVersion : text
            color: Theme.color.lightorange3
            visible: extraFields
        }
    }
}
