import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0
import QFlipper 1.0

AbstractOverlay {
    id: overlay

    TextLabel {
        id: updateLabel
        capitalized: false
        anchors.horizontalCenter: parent.horizontalCenter
        y: 24

        color: Theme.color.lightorange2

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        text: qsTr("Updating qFlipper")
    }

    ProgressBar {
        id: progressBar

        width: 280
        height: 56

        from: 0
        to: 100

        x: Math.round((parent.width - width) / 2)
        y: 265

        value: app.updater.progress
        indeterminate: value < 0
    }

    TextLabel {
        id: messageLabel
        anchors.top: progressBar.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter

        text: {
            switch(app.updater.state) {
            case AppUpdater.Downloading:
                return qsTr("Downloading latest version...");
            case AppUpdater.Updating:
                return qsTr("Starting update process...");
            case AppUpdater.ErrorOccured:
                return qsTr("Update failed");
            default:
                return qsTr("Preparing...");
            }
        }
        color: Theme.color.lightorange2
    }
}
