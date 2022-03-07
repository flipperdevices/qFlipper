import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0
import QFlipper 1.0

Item {
    id: delegate

    required property int index
    required property string fileName
    required property string filePath
    required property int fileType
    required property int fileSize

    readonly property bool isDirectory: !fileType
    readonly property bool isHovered: mouseArea.containsMouse
    readonly property bool isCurrent: GridView.isCurrentItem
    readonly property color selectionColor: Color.transparent(Theme.color.darkorange1, delegate.isCurrent ? 1 : delegate.isHovered ? 0.5 : 0)

    width: 120
    height: 86

    ColorAnimation {
        id: selectionAnimation
        duration: 150
        easing.type: Easing.OutQuad
    }

    ColumnLayout {
        spacing: 3
        anchors.fill: parent

        Rectangle {
            Layout.topMargin: 10
            Layout.alignment: Qt.AlignHCenter

            radius: 2
            color: delegate.selectionColor
            width: icon.width + 12
            height: icon.height + 12

            Behavior on color {
                animation: selectionAnimation
            }

            IconImage {
                id: icon

                x: 6
                y: 6

                layer.enabled: true

                sourceSize: Qt.size(28, 28)
                color: Theme.color.lightorange2

                source: {
                    if(delegate.filePath === "/ext") {
                        return "qrc:/assets/gfx/symbolic/mimetypes/sdcard.svg";
                    } else if(delegate.filePath === "/int") {
                        return "qrc:/assets/gfx/symbolic/mimetypes/internal.svg";
                    } else if(delegate.isDirectory) {
                        return "qrc:/assets/gfx/symbolic/mimetypes/folder.svg";
                    }

                    const extension = delegate.fileName.substring(delegate.fileName.lastIndexOf('.') + 1);

                    // TODO: move to a dictionary?
                    if(extension === "ir") {
                        return "qrc:/assets/gfx/symbolic/mimetypes/infrared.svg";
                    } else if(extension === "nfc") {
                        return "qrc:/assets/gfx/symbolic/mimetypes/nfc.svg";
                    } else if(extension === "u2f") {
                        return "qrc:/assets/gfx/symbolic/mimetypes/u2f.svg";
                    } else {
                        return "qrc:/assets/gfx/symbolic/mimetypes/file.svg";
                    }
                }
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: delegate.width - 8

            text: {
                if(delegate.filePath === "/ext") {
                    return "SD Card";
                } else if(delegate.filePath === "/int") {
                    return "Internal Flash";
                } else {
                    delegate.fileName
                }
            }

            font.pixelSize: 16
            font.letterSpacing: -1
            font.family: "Share Tech Mono"

            color: Theme.color.lightorange2
            horizontalAlignment: Text.AlignHCenter

            maximumLineCount: 2
            elide: Text.ElideRight
            wrapMode: Text.WrapAnywhere
            textFormat: Text.PlainText

            Rectangle {
                z: parent.z - 1
                anchors.fill: parent
                color: delegate.selectionColor

                Behavior on color {
                    animation: selectionAnimation
                }
            }
        }

        Item { Layout.fillHeight: true }
    }

    Item {
        id: draggable
        anchors.fill: parent

        Drag.active: mouseArea.drag.active
        Drag.mimeData: { "text/uri-list": Qt.resolvedUrl("file:///home/test/hello.txt") }
        Drag.dragType: Drag.Automatic
        Drag.proposedAction: Qt.CopyAction
        Drag.supportedActions: Qt.CopyAction

//        Drag.onDragFinished: function(dropAction) {
//        }
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        drag.target: draggable
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: {
            delegate.GridView.view.currentIndex = delegate.index

            if(mouse.button === Qt.LeftButton) {
                return;
            }

            if(delegate.filePath === "/ext" || delegate.filePath === "/int") {
                storageMenu.popup();
            } else if(delegate.isDirectory) {
                dirMenu.popup();
            } else {
                fileMenu.popup();
            }
        }

        onDoubleClicked: {
            if(mouse.button === Qt.RightButton || !delegate.isDirectory) {
                return;
            }

            Backend.fileManager.pushd(delegate.fileName)
        }
    }

    Menu {
        id: storageMenu

        MenuItem {
            action: uploadHereAction
        }

        MenuItem {
            action: saveAction
        }
    }

    Menu {
        id: fileMenu

        MenuItem {
            action: saveAction
        }

        MenuItem {
            action: renameAction
        }

        MenuItem {
            action: removeAction
        }
    }

    Menu {
        id: dirMenu

        MenuItem {
            action: uploadHereAction
        }

        MenuItem {
            action: saveAction
        }

        MenuItem {
            action: renameAction
        }

        MenuItem {
            action: removeAction
        }
    }

    Action {
        id: uploadHereAction
        text: qsTr("Upload here")
    }

    Action {
        id: saveAction
        text: qsTr("Download")
    }

    Action {
        id: renameAction
        text: qsTr("Rename")
    }

    Action {
        id: removeAction
        text: qsTr("Delete")
    }
}
