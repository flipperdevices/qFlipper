import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

    closable: true
    invertTitle: true
    title: firmwareUpdates.isReady ? qsTr("Version %1 changelog").arg(firmwareUpdates.latestVersion.number) : qsTr("No data")

    contentWidget: TextView {
        text: firmwareUpdates.isReady ? firmwareUpdates.latestVersion.changelog : qsTr("No data")
        textFormat: Text.MarkdownText

        implicitWidth: control.width - 70
        implicitHeight: control.height - 160

        background.visible: false
    }
}
