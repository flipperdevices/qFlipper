import QtQuick 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: control
    spacing: 30

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

    Item {
        Layout.fillWidth: true
    }

    ColumnLayout {
        id: keys

        TextLabel {
            id: versionLabel
            text: qsTr("Version")
            Layout.alignment: Qt.AlignRight
            visible: deviceState ? !deviceState.isRecoveryMode : visible
        }

        TextLabel {
            text: qsTr("Date")
            Layout.alignment: Qt.AlignRight
            visible: versionLabel.visible
        }

        TextLabel {
            text: qsTr("Hardware")
            Layout.alignment: Qt.AlignRight
        }

        TextLabel {
            text: qsTr("Battery")
            Layout.alignment: Qt.AlignRight
            visible: versionLabel.visible
        }
    }

    ColumnLayout {
        id: values

        TextLabel {
            text: !deviceInfo ? text : deviceInfo.firmware.branch === "dev" ?
                   deviceInfo.firmware.commit : deviceInfo.firmware.version

            visible: versionLabel.visible
        }

        TextLabel {
            text: deviceInfo ? deviceInfo.firmware.date.toLocaleDateString(Qt.locale("C"), Locale.ShortFormat) : text
            visible: versionLabel.visible
        }

        TextLabel {
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
        }

        TextLabel {
            text: "N/A"
            visible: versionLabel.visible
        }
    }

    Item {
        Layout.fillWidth: true
    }
}
