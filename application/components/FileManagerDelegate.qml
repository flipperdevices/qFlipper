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

        Item {
            Layout.alignment: Qt.AlignHCenter

            implicitWidth: delegate.width - 8
            implicitHeight: nameLabel.height

            Rectangle {
                anchors.fill: nameLabel
                color: delegate.selectionColor

                Behavior on color {
                    animation: selectionAnimation
                }
            }

            Text {
                id: nameLabel

                visible: !editBox.visible
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 16
                font.letterSpacing: -1
                font.family: "Share Tech Mono"

                color: Theme.color.lightorange2
                horizontalAlignment: Text.AlignHCenter

                maximumLineCount: 2
                elide: Text.ElideRight
                wrapMode: Text.WrapAnywhere
                textFormat: Text.PlainText

                width: Math.min(implicitWidth, delegate.width - 8)

                text: {
                    if(delegate.filePath === "/ext") {
                        return "SD Card";
                    } else if(delegate.filePath === "/int") {
                        return "Internal Flash";
                    } else {
                        delegate.fileName
                    }
                }
            }

            Rectangle {
                id: editBox

                readonly property int padding: 4

                visible: false

                y: -padding
                width: parent.width
                height: nameEdit.height + padding * 2

                color: "black"
                border.color: Theme.color.lightorange2
                border.width: 1
                radius: 2

                onVisibleChanged: {
                    if(visible) {
                        nameEdit.text = nameLabel.text
                        nameEdit.selectAll();
                        nameEdit.forceActiveFocus(Qt.MouseFocusReason);
                    }
                }

                TextInput {
                    id: nameEdit

                    y: editBox.padding

                    width: parent.width
                    font: nameLabel.font

                    color: Theme.color.lightorange2
                    selectionColor: color
                    selectedTextColor: "black"

                    wrapMode: Text.WrapAnywhere
                    horizontalAlignment: Text.AlignHCenter

                    selectByMouse: true

                    onAccepted: {
                        console.log("Renaming...")
                    }

                    onEditingFinished: {
                        editBox.visible = false;
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
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
        text: qsTr("Upload here...")
    }

    Action {
        id: saveAction
        text: qsTr("Download...")
    }

    Action {
        id: renameAction
        text: qsTr("Rename")
        onTriggered: editBox.visible = true;
    }

    Action {
        id: removeAction
        text: qsTr("Delete")
    }
}
