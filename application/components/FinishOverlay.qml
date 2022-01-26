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
        visible: Backend.backendState === Backend.ErrorOccured

        anchors.fill: parent
        anchors.margins: 36

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextLabel {
                id: errorLabel

                Layout.alignment: Qt.AlignHCenter

                capitalized: false
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
                        return qsTr("Recovery Error")
                    case BackendError.ProtocolError:
                        return qsTr("Communication Error")
                    case BackendError.DiskError:
                        return qsTr("Disk Access Error")
                    case BackendError.TimeoutError:
                        return qsTr("Timeour Error")
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
                    source: Backend.errorType === BackendError.SerialError ||
                            Backend.errorType === BackendError.RecoveryError ? "qrc:/assets/gfx/images/error-access.svg" :
                                                                               "qrc:/assets/gfx/images/error-client.svg"

                    visible: !flipperError.visible
                }

                Image {
                    id: flipperError
                    visible: Backend.errorType === BackendError.InvalidDevice ||
                             Backend.errorType === BackendError.UnknownError

                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -30
                    anchors.horizontalCenterOffset: -15

                    sourceSize: Qt.size(360, 156)
                    source: "qrc:/assets/gfx/images/flipper.svg"

                    Image {
                        x: 93
                        y: 26

                        source: Backend.errorType === BackendError.InvalidDevice ? "qrc:/assets/gfx/images/error-exclamation.svg" :
                                                                                   "qrc:/assets/gfx/images/error-cross-eyes.svg"
                        sourceSize: Qt.size(128, 64)
                    }
                }
            }
        }

        TextBox {
            Layout.fillHeight: true
            Layout.preferredWidth: 335

            style: Strings.errorStyle
            text: {
                switch(Backend.errorType) {
                case BackendError.InvalidDevice:
                    return Strings.errorInvalidDevice
                case BackendError.SerialError:
                    return Strings.errorSerial
                case BackendError.RecoveryError:
                    return Strings.errorRecovery
                case BackendError.UnknownError:
                default:
                    return Strings.errorUnknown
                }
            }
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
