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

    onVisibleChanged: {
        if(Backend.backendState === ApplicationBackend.Ready) {
            Backend.screenStreamer.isEnabled = !visible
        }
    }

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

                        font.pixelSize: 18
                        font.family: "Share Tech"

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

            Switch {
                id: hiddenFileSwitch
                text: qsTr("Hidden files")
                checked: Preferences.showHiddenFiles
                onCheckedChanged: {
                    Preferences.showHiddenFiles = checked;
                    Backend.fileManager.refresh();
                }
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

                    onClicked: function(mouse) {
                        fileView.currentIndex = -1;
                        forceActiveFocus(Qt.MouseFocusReason);

                        if(mouse.button === Qt.RightButton && !Backend.fileManager.isRoot) {
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
        enabled: !Backend.fileManager.isRoot
        anchors.fill: parent
        onDropped: function(drop) {
            if(drop.source || drop.proposedAction !== Qt.CopyAction) {
                return;
            }

            control.uploadUrls(drop.urls);
            drop.accept()
        }
    }

    Menu {
        id: emptyMenu

        MenuItem { action: uploadHereAction }
        MenuItem { action: newDirAction }
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
        icon.source: "qrc:/assets/gfx/symbolic/filemgr/action-upload.svg"

        onTriggered: {
            SystemFileDialog.accepted.connect(function() {
                control.uploadUrls(SystemFileDialog.fileUrls);
            });

            SystemFileDialog.beginOpenFiles(SystemFileDialog.LastLocation, [ "All files (*)" ]);
        }
    }

    Action {
        id: newDirAction
        text: qsTr("New Folder")
        onTriggered: Backend.fileManager.beginMkDir();
        icon.source: "qrc:/assets/gfx/symbolic/filemgr/action-new.svg"
    }

    Component.onCompleted: {
        Backend.fileManager.currentPathChanged.connect(function() {
            fileView.currentIndex = -1;
        });
    }

    function uploadUrls(urls) {
        const doUpload = function() {
            Backend.fileManager.upload(urls);
        };

        if(Backend.fileManager.isTooLarge(urls)) {
            const isMultiple = urls.length > 1;
            const msgObj = {
                title: qsTr("Warning"),
                message: qsTr("Selected %1 too large.\nUpload anyway?").arg(isMultiple ? qsTr("files are") : qsTr("file is")),
                suggestedRole: ConfirmationDialog.RejectRole,
                customText: qsTr("Upload")
            };

            confirmationDialog.openWithMessage(doUpload, msgObj);

        } else {
            doUpload();
        }
    }
}
