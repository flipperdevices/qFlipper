import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

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

        text: qsTr("Updating your Flipper")
    }

    ProgressBar {
        id: progressBar

        width: 280
        height: 56

        from: 0
        to: 100

        x: Math.round((parent.width - width) / 2)
        y: 265

        value: deviceState ? deviceState.progress : 0
        indeterminate: !deviceState ? true : deviceState.progress < 0
    }

    TextLabel {
        id: messageLabel
        anchors.top: progressBar.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        text: !deviceState ? text : deviceState.isError ? deviceState.errorString : deviceState.statusString
        color: Theme.color.lightorange2
    }

    ColumnLayout {
        x: 620
        y: 120

        opacity: !deviceInfo ? 0 : !deviceState.isRecoveryMode && !deviceInfo.storage.isExternalPresent ? 1 : 0

        Image {
            source: "qrc:/assets/gfx/images/no-sd-card.svg"
            sourceSize: Qt.size(44, 58)
            Layout.alignment: Qt.AlignHCenter
        }

        TextLabel {
            text: qsTr("No sd")
            Layout.alignment: Qt.AlignHCenter
        }

        Behavior on opacity {
            PropertyAnimation {
                duration: 150
                easing.type: Easing.InOutQuad
            }
        }
    }
}
