import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0
import QFlipper 1.0

CustomDialog {
    id: control

    closable: App.updater.state === ApplicationUpdater.ErrorOccured
    closePolicy: Popup.NoAutoClose

    title: App.updater.state === ApplicationUpdater.Idle ? qsTr("Update qFlipper?") : qsTr("Updating qFlipper")

    contentWidget: Item {
        implicitWidth: 430
        implicitHeight: layout.implicitHeight

        ColumnLayout {
            id: layout
            width: parent.implicitWidth

            TextLabel {
                id: messageLabel
                visible: App.updater.state === ApplicationUpdater.Idle
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: qsTr("Newer version of qFlipper<br/>will be installed")

                lineHeight: 1.4
                wrapMode: Text.Wrap

                Layout.topMargin: 24
                Layout.bottomMargin: -8
                Layout.fillWidth: true
            }

            // TODO: Find a way to use a DialogButtonBox properly

            RowLayout {
                id: buttonBox
                visible: App.updater.state === ApplicationUpdater.Idle
                spacing: 30
                Layout.margins: 20
                Layout.fillWidth: true
                Layout.preferredHeight: 42
                layoutDirection: Qt.platform.os === "osx" ? Qt.RightToLeft : Qt.LeftToRight

                SmallButton {
                    radius: 7
                    text: qsTr("Update")
                    highlighted: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: App.selfUpdate();
                }

                SmallButton {
                    radius: 7
                    text: qsTr("Cancel")
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: control.rejected()
                }
            }

            ProgressBar {
                id: progressBar

                visible: App.updater.state !== ApplicationUpdater.Idle

                implicitWidth: 286
                implicitHeight: 56

                from: 0
                to: 100

                value: App.updater.progress
                indeterminate: value < 0

                Layout.topMargin: 40
                Layout.bottomMargin: 20
                Layout.alignment: Qt.AlignHCenter
            }

            TextLabel {
                id: progressLabel
                padding: 0

                visible: App.updater.state !== ApplicationUpdater.Idle

                text: {
                    switch(App.updater.state) {
                    case ApplicationUpdater.Downloading:
                        return qsTr("Downloading latest version...");
                    case ApplicationUpdater.Updating:
                        return qsTr("Starting update process...");
                    case ApplicationUpdater.ErrorOccured:
                        return qsTr("Update failed (See log).");
                    default:
                        return qsTr("Preparing...");
                    }
                }

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                Layout.fillWidth: true
                Layout.bottomMargin: 14

                color: App.updater.state === ApplicationUpdater.ErrorOccured ? Theme.color.lightred3 : Theme.color.lightorange2
            }
        }
    }
}
