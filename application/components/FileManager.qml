import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0
import QFlipper 1.0

Item {
    id: control

    implicitWidth: 745
    implicitHeight: 290

    property ConfirmationDialog confirmationDialog
    readonly property bool isRoot: Backend.fileManager.currentPath === "/"

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 7
        anchors.rightMargin: 7

        RowLayout {
            spacing: 7
            Layout.fillWidth: true

            Button {
                id: backButton
                action: backAction

                implicitWidth: 30
                implicitHeight: 30

                padding: 4
            }

            Button {
                id: fwdButton
                action: forwardAction

                implicitWidth: 30
                implicitHeight: 30

                padding: 5
            }

            Rectangle {
                width: 500
                height: 30

                color: "black"
                radius: 6

                border.width: 2
                border.color: Theme.color.mediumorange1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 7

                    spacing: 5

                    IconImage {
                        Layout.alignment: Qt.AlignVCenter

                        color: Theme.color.lightorange2
                        sourceSize: Qt.size(16, 16)
                        source: {
                            const path = Backend.fileManager.currentPath

                            if(path.startsWith("/ext")) {
                                return "qrc:/assets/gfx/symbolic/filemgr/location-sdcard.svg"
                            } else if(path.startsWith("/int")) {
                                return "qrc:/assets/gfx/symbolic/filemgr/location-internal.svg"
                            } else {
                                return "";
                            }
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter

                        font.pixelSize: 16
                        font.letterSpacing: -1
                        font.family: "Share Tech Mono"

                        color: Theme.color.lightorange2
                        elide: Text.ElideMiddle

                        text: {
                            const path = Backend.fileManager.currentPath

                            if(path.startsWith("/ext")) {
                                return path.replace("/ext", "SD Card") + "/";
                            } else if(path.startsWith("/int")) {
                                return path.replace("/int", "Internal Flash") + "/";
                            } else {
                                return path;
                            }
                        }

                        onTextChanged: {
                            fileView.currentIndex = -1;
                        }

                    }
                }
            }

            Button {
                id: refreshButton
                action: refreshAction

                implicitWidth: 30
                implicitHeight: 30

                padding: 2
            }
        }

        ScrollView {
            id: scrollView

            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollBar.vertical.anchors.right: scrollView.right
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            padding: 0
            clip: true

            background: Item {}

            GridView {
                id: fileView
                cellWidth: 120
                cellHeight: 86

                MouseArea {
                    z: parent.z - 1
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked: {
                        fileView.currentIndex = -1;
                        forceActiveFocus(Qt.MouseFocusReason);

                        if(mouse.button === Qt.RightButton && !control.isRoot) {
                            emptyMenu.popup();
                        }
                    }
                }

                model: Backend.fileManager
                delegate: FileManagerDelegate {
                    confirmationDialog: control.confirmationDialog
                }
            }
        }
    }

    DropArea {
        enabled: !control.isRoot
        anchors.fill: parent
        onDropped: {
            if(drop.source || drop.proposedAction !== Qt.CopyAction) {
                return;
            }

            Backend.fileManager.upload(drop.urls);
            drop.accept()
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: Backend.fileManager.isBusy
        color: Color.transparent("black", visible ? 0.8 : 0)

        Behavior on color {
            PropertyAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }

        MouseArea {
            acceptedButtons: Qt.AllButtons
            anchors.fill: parent
        }

        ProgressBar {
            id: progressBar

            anchors.centerIn: parent

            width: 280
            height: 56

            from: 0
            to: 100

            value: Backend.deviceState ? Backend.deviceState.progress : 0
            indeterminate: Backend.deviceState ? Backend.deviceState.progress < 0 : true
        }

        TextLabel {
            id: messageLabel
            anchors.top: progressBar.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Working, please wait...")
        }
    }

    Menu {
        id: emptyMenu
        MenuItem {
            action: uploadHereAction
        }

        MenuItem {
            action: newDirAction
        }
    }

    Action {
        id: forwardAction
        enabled: Backend.fileManager.canGoForward

        icon.width: 8
        icon.height: 14
        icon.source: "qrc:/assets/gfx/symbolic/arrow-forward-small.svg"

        onTriggered: Backend.fileManager.historyForward()
    }

    Action {
        id: backAction
        enabled: Backend.fileManager.canGoBack

        icon.width: 8
        icon.height: 14
        icon.source: "qrc:/assets/gfx/symbolic/arrow-back-small.svg"

        onTriggered: Backend.fileManager.historyBack()
    }

    Action {
        id: refreshAction

        icon.width: 16
        icon.height: 16
        icon.source: "qrc:/assets/gfx/symbolic/refresh-small.svg"

        onTriggered: Backend.fileManager.refresh()
    }

    Action {
        id: uploadHereAction
        text: qsTr("Upload here...")

        onTriggered: {
            const onFinished = function() {
                AdvancedFileDialog.accepted.disconnect(onAccepted);
                AdvancedFileDialog.finished.disconnect(onFinished);
            };

            const onAccepted = function() {
                Backend.fileManager.upload(AdvancedFileDialog.fileUrls);
            };

            AdvancedFileDialog.accepted.connect(onAccepted);
            AdvancedFileDialog.finished.connect(onFinished);

            AdvancedFileDialog.defaultFileName = "";
            AdvancedFileDialog.title = qsTr("Select files to upload");
            AdvancedFileDialog.nameFilters = [ "All files (*)" ];
            AdvancedFileDialog.openLocation = AdvancedFileDialog.HomeLocation;
            AdvancedFileDialog.selectExisting = true;
            AdvancedFileDialog.selectMultiple = true;
            AdvancedFileDialog.selectFolder = false;

            AdvancedFileDialog.exec();
        }
    }

    Action {
        id: newDirAction
        text: qsTr("New Folder...")
        onTriggered: Backend.fileManager.beginMkDir();
    }

    Component.onCompleted: {
        Backend.fileManager.refreshed.connect(function() {
            fileView.currentIndex = -1;
        });
    }
}
