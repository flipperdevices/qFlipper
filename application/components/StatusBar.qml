import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0
import QFlipper 1.0

Rectangle {
    id: control

    readonly property bool errorOccured: Backend.backendState === Backend.ErrorOccured ||
                                         Backend.firmwareUpdateState === Backend.ErrorOccured

    color: errorOccured ? Theme.color.darkred2 : Theme.color.darkorange1

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        spacing: 10

        IconImage {
            color: message.color
            sourceSize: Qt.size(22, 22)
            source: "qrc:/assets/gfx/symbolic/info-small.svg"
            rotation: errorOccured ? 180 : 0
        }

        TextLabel {
            id: message

            text: Backend.backendState === ApplicationBackend.ErrorOccured ?
                  Backend.errorType === BackendError.InvalidDevice ? qsTr("Unsupported device found") :
                  Backend.errorType === BackendError.InternetError ? qsTr("Cannot connect to update server") :
                  Backend.errorType === BackendError.SerialAccessError ? qsTr("Cannot access serial port"):
                  Backend.errorType === BackendError.RecoveryAccessError ? qsTr("Cannot access device in recovery mode"):
                  Backend.errorType === BackendError.DiskError ? qsTr("Disk access error"):
                  Backend.errorType === BackendError.BackupError ? qsTr("Cannot save Flipper data"):
                  Backend.errorType === BackendError.OperationError ? qsTr("Operation interrupted"):
                  Backend.errorType === BackendError.DataError ? qsTr("Data corrupted"): qsTr("Something went wrong. Check logs for details.") :

                  Backend.backendState === ApplicationBackend.WaitingForDevices ? qsTr("Waiting for devices ...") :
                  Backend.backendState > ApplicationBackend.ScreenStreaming && Backend.backendState < ApplicationBackend.Finished ? qsTr("Do not unplug the device ...") :
                  Backend.backendState === ApplicationBackend.Finished ? qsTr("Operation has finished successfully.") :
                  Backend.firmwareUpdateState === ApplicationBackend.ErrorOccured ? qsTr("Cannot connect to update server") :
                  Backend.firmwareUpdateState === ApplicationBackend.Checking ? qsTr("Checking for firmware updates...") : qsTr("Ready.")

            color: control.errorOccured ? Theme.color.lightred4 : Theme.color.lightorange2

            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
        }
    }

    Behavior on color {
        ColorAnimation {
            duration: 150
            easing.type: Easing.OutQuad
        }
    }
}
