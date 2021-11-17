import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

    closable: true
    invertTitle: false
    title: firmwareUpdates.isReady ? qsTr("Version %1 changelog").arg(firmwareUpdates.latestVersion.number) : qsTr("No data")

    contentWidget: ScrollView {
        id: scrollView
        clip: true

        implicitWidth: control.width - 70
        implicitHeight: control.height - 168

        background.visible: false
        contentWidth: availableWidth

        Text {
            width: scrollView.availableWidth
            text: firmwareUpdates.isReady ? firmwareUpdates.latestVersion.changelog : qsTr("No data")
            textFormat: Text.MarkdownText
            color: Theme.color.lightorange2
            wrapMode: Text.Wrap
        }
    }
}
