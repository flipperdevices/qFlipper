import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0
import QFlipper 1.0

CustomDialog {
    id: control

    closable: true
    invertTitle: true
    title: !(Backend.firmwareUpdateState === Backend.Unknown ||
             Backend.firmwareUpdateState === Backend.Checking ||
             Backend.firmwareUpdateState === Backend.ErrorOccured) ?
           qsTr("Version %1 changelog").arg(Backend.latestFirmwareVersion.number) : qsTr("No data")

    contentWidget: TextView {
        text: !(Backend.firmwareUpdateState === Backend.Unknown ||
                Backend.firmwareUpdateState === Backend.Checking ||
                Backend.firmwareUpdateState === Backend.ErrorOccured) ?
              Backend.latestFirmwareVersion.changelog : qsTr("No data")

        textFormat: Text.MarkdownText

        implicitWidth: control.width - 70
        implicitHeight: control.height - 160

        background.visible: false
    }
}
