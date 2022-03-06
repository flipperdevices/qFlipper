import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0
import QFlipper 1.0
import Strings 1.0

ItemDelegate {
    id: delegate

    required property string fileName
    required property string filePath
    required property int fileType
    required property int fileSize

    width: 120
    height: 86

    ColumnLayout {
        anchors.fill: parent

        IconImage {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10

            source: {
                if(delegate.filePath === "/ext") {
                    return "qrc:/assets/gfx/symbolic/mimetypes/sdcard.svg";
                } else if(delegate.filePath === "/int") {
                    return "qrc:/assets/gfx/symbolic/mimetypes/internal.svg";
                } else if(delegate.fileType === 0) {
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

            sourceSize: Qt.size(28, 28)
            color: Theme.color.lightorange2
        }

        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.leftMargin: 4
            Layout.rightMargin: 4

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
        }
    }

    DragHandler {
        id: dragHandler
        target: null
        dragThreshold: 3
    }

    Item {
        id: draggable
        anchors.fill: parent

        Drag.active: dragHandler.active
        Drag.mimeData: { "text/plain": delegate.filePath }
        Drag.dragType: Drag.Automatic
        Drag.onDragFinished: function(dropAction) {
            if(dropAction !== Qt.MoveAction) {
                return;
            }

            console.log("Uploading", delegate.fileName)
        }
    }

    onClicked: {
        if(fileType !== 0) {
            return;
        }

        Backend.fileManager.pushd(delegate.fileName)
    }
}
