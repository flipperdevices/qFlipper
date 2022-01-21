import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import QFlipper 1.0
import Strings 1.0
import Theme 1.0

AbstractOverlay {
    id: overlay

    TextLabel {
        id: successLabel

        y: 24

        capitalized: false
        visible: Backend.backendState === Backend.Finished

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        anchors.horizontalCenter: parent.horizontalCenter

        text: qsTr("Success!")
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 36

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextLabel {
                id: errorLabel

                Layout.alignment: Qt.AlignHCenter

                capitalized: false
                visible: Backend.backendState === Backend.ErrorOccured

                font.family: "Born2bSportyV2"
                font.pixelSize: 48

                text: {
                    switch(Backend.errorType) {
                    case BackendError.InvalidDevice:
                        return qsTr("Invalid device")
                    case BackendError.InternetError:
                        return qsTr("Internet Error")
                    case BackendError.SerialError:
                        return qsTr("Serial Port Error")
                    case BackendError.RecoveryError:
                        return qsTr("USB Connection Error")
                    case BackendError.ProtocolError:
                        return qsTr("Communication Error")
                    case BackendError.DiskError:
                        return qsTr("Disk Access Error")
                    case BackendError.UnknownError:
                    default:
                        return qsTr("Unknown Error")
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Image {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -15

                    sourceSize: Qt.size(246, 187)
                    source: "qrc:/assets/gfx/images/error-client.svg"
                }
            }
        }

        TextBox {
            Layout.fillHeight: true
            Layout.preferredWidth: 335

            visible: Backend.backendState === Backend.ErrorOccured

            style: Strings.errorStyle
            text: Strings.errorRecovery
        }
    }

    Button {
        id: backButton
        action: backAction

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 25

        icon.width: 24
        icon.height: 24
        icon.source: "qrc:/assets/gfx/symbolic/arrow-back.svg"

        visible: Backend.backendState === Backend.ErrorOccured
    }

    MainButton {
        id: continueButton
        action: continueAction
        anchors.horizontalCenter: parent.horizontalCenter
        visible: Backend.backendState === Backend.Finished
        focus: visible
        y: 265

        Keys.onPressed: continueAction.trigger()
    }

    Action {
        id: backAction
        text: qsTr("Back")
        onTriggered: Backend.finalizeOperation()
    }

    Action {
        id: continueAction
        text: qsTr("Continue")
        onTriggered: Backend.finalizeOperation()
    }
}
