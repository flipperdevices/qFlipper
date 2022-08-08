import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import QFlipper 1.0
import Theme 1.0
import Misc 1.0

AbstractOverlay {
    id: overlay

    TextLabel {
        id: successLabel

        y: 19

        capitalized: false
        visible: Backend.backendState === ApplicationBackend.Finished

        font.family: "Born2bSportyV2"
        font.pixelSize: 48

        anchors.horizontalCenter: parent.horizontalCenter

        text: qsTr("Success!")
    }

    RowLayout {
        visible: Backend.backendState === ApplicationBackend.ErrorOccured

        anchors.fill: parent
        anchors.margins: 36
        anchors.leftMargin: 0

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
                    case BackendError.DiskError:
                        return qsTr("Disk Access Error")
                    case BackendError.SerialAccessError:
                        return qsTr("Permission Denied")
                    case BackendError.RecoveryAccessError:
                        return qsTr("Can't Find DFU Device")
                    case BackendError.BackupError:
                        return qsTr("Backup Failed")
                    case BackendError.FileError:
                        return qsTr("Corrupted Data")
                    case BackendError.OperationError:
                        return qsTr("Operation Error")
                    case BackendError.UpdaterError:
                        return qsTr("Updater Error")
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
                    source: Backend.errorType === BackendError.SerialAccessError ||
                            Backend.errorType === BackendError.RecoveryAccessError ? "qrc:/assets/gfx/images/error-access.svg" :
                            Backend.errorType === BackendError.InternetError ? "qrc:/assets/gfx/images/error-internet.svg" :
                                                                               "qrc:/assets/gfx/images/error-client.svg"

                    visible: !flipperError.visible
                }

                Image {
                    id: flipperError
                    visible: Backend.errorType === BackendError.InvalidDevice  ||
                             Backend.errorType === BackendError.OperationError ||
                             Backend.errorType === BackendError.BackupError ||
                             Backend.errorType === BackendError.UpdaterError ||
                             Backend.errorType === BackendError.UnknownError

                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -30
                    anchors.horizontalCenterOffset: -10

                    sourceSize: Qt.size(360, 156)
                    source: "qrc:/assets/gfx/images/flipper.svg"

                    Image {
                        x: 93
                        y: 26

                        source: Backend.errorType === BackendError.OperationError ? "qrc:/assets/gfx/images/error-cross-eyes.svg" :
                                                                                    "qrc:/assets/gfx/images/error-exclamation.svg"
                        sourceSize: Qt.size(128, 64)
                    }
                }
            }
        }

        TextBox {
            Layout.preferredWidth: 335
            Layout.alignment: Qt.AlignVCenter

            style: ErrorStrings.errorStyle
            text: {
                switch(Backend.errorType) {
                case BackendError.InternetError:
                    return ErrorStrings.errorInternet
                case BackendError.InvalidDevice:
                    return ErrorStrings.errorInvalidDevice
                case BackendError.DiskError:
                    return ErrorStrings.errorDisk
                case BackendError.SerialAccessError:
                    return Qt.platform.os === "linux" ? ErrorStrings.errorSerialLinux : ErrorStrings.errorSerial
                case BackendError.RecoveryAccessError:
                    return Qt.platform.os === "linux" ? ErrorStrings.errorRecoveryLinux :
                           Qt.platform.os === "windows" ? ErrorStrings.errorRecoveryWindows :
                                                          ErrorStrings.errorRecovery
                case BackendError.BackupError:
                    return ErrorStrings.errorBackup
                case BackendError.DataError:
                    return ErrorStrings.errorData
                case BackendError.OperationError:
                    return ErrorStrings.errorOperation
                case BackendError.UpdaterError:
                    return ErrorStrings.errorUpdater
                default:
                    return ErrorStrings.errorUnknown
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

        visible: Backend.backendState === ApplicationBackend.ErrorOccured
    }

    MainButton {
        id: continueButton
        action: continueAction
        anchors.horizontalCenter: parent.horizontalCenter
        visible: Backend.backendState === ApplicationBackend.Finished
        focus: visible
        y: 270

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
