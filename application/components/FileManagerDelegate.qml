import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Misc 1.0
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
    readonly property bool isNewDirectory: Backend.fileManager.newDirectoryIndex === index
    readonly property bool isHovered: iconMouseArea.containsMouse || labelMouseArea.containsMouse
    readonly property bool isCurrent: GridView.isCurrentItem

    property color selectionColor: Color.transparent(Theme.color.darkorange1, delegate.isCurrent ? 1 : delegate.isHovered ? 0.5 : 0)

    property ConfirmationDialog confirmationDialog

    width: 120
    height: 86

    onIsNewDirectoryChanged: {
        if(isNewDirectory) {
            beginEdit();
        }
    }

    Behavior on selectionColor {
        ColorAnimation {
            duration: 150
            easing.type: Easing.OutQuad
        }
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

            MouseArea {
                id: iconMouseArea
                hoverEnabled: true
                anchors.fill: parent

                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked: delegate.rightClick(mouse);
                onDoubleClicked: delegate.doubleClick(mouse)
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

                    if(extension in FileTypes.icons) {
                        return FileTypes.icons[extension];
                    } else {
                        return FileTypes.icons["default"];
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
            }

            MouseArea {
                id: labelMouseArea
                hoverEnabled: true
                anchors.fill: parent

                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked: delegate.rightClick(mouse);
                onDoubleClicked: delegate.doubleClick(mouse)
            }

            Text {
                id: nameLabel

                visible: !editBox.visible
                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: 16
                font.family: "Share Tech"

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

                readonly property int padding: 2

                visible: false

                y: -padding
                width: parent.width
                height: nameEdit.height + padding * 2

                color: "black"
                border.color: Theme.color.lightorange2
                border.width: 1

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

                    onEditingFinished: delegate.commitEdit();

                    validator: RegularExpressionValidator {
                        regularExpression: /[\x20-\x7E]+/ //Printable ASCII characters
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }

    Menu {
        id: storageMenu

        MenuItem { action: uploadHereAction }
        MenuItem { action: downloadAction }
    }

    Menu {
        id: fileMenu

        MenuItem { action: downloadAction }
        MenuItem { action: renameAction }
        MenuItem { action: removeAction }
    }

    Menu {
        id: dirMenu

        MenuItem { action: uploadHereAction }
        MenuItem { action: downloadAction }
        MenuItem { action: renameAction }
        MenuItem { action: removeAction }
    }

    Action {
        id: uploadHereAction
        text: qsTr("Upload here...")
        icon.source: "qrc:/assets/gfx/symbolic/filemgr/action-upload.svg"

        onTriggered: {
            const onFinished = function() {
                AdvancedFileDialog.accepted.disconnect(onAccepted);
                AdvancedFileDialog.finished.disconnect(onFinished);
            };

            const onAccepted = function() {
                const doUpload = function() {
                    Backend.fileManager.uploadTo(delegate.fileName, AdvancedFileDialog.fileUrls);
                };

                if(Backend.fileManager.isTooLarge(AdvancedFileDialog.fileUrls)) {
                    const isMultiple = AdvancedFileDialog.fileUrls.length > 1;
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
        id: downloadAction
        text: qsTr("Download...")
        icon.source: "qrc:/assets/gfx/symbolic/filemgr/action-download.svg"

        onTriggered: {
            const onFinished = function() {
                AdvancedFileDialog.accepted.disconnect(onAccepted);
                AdvancedFileDialog.finished.disconnect(onFinished);
            };

            const onAccepted = function() {
                Backend.fileManager.download(delegate.fileName, AdvancedFileDialog.fileUrls[0], delegate.isDirectory);
            };

            AdvancedFileDialog.accepted.connect(onAccepted);
            AdvancedFileDialog.finished.connect(onFinished);

            AdvancedFileDialog.defaultFileName = delegate.isDirectory ? "" : delegate.fileName;
            AdvancedFileDialog.title = qsTr("Select download location");
            AdvancedFileDialog.nameFilters = [ "All files (*)" ];
            AdvancedFileDialog.openLocation = AdvancedFileDialog.DownloadsLocation;
            AdvancedFileDialog.selectMultiple = false;
            AdvancedFileDialog.selectExisting = delegate.isDirectory;
            AdvancedFileDialog.selectFolder = delegate.isDirectory;

            AdvancedFileDialog.exec();
        }
    }

    Action {
        id: renameAction
        text: qsTr("Rename")
        icon.source: "qrc:/assets/gfx/symbolic/filemgr/action-rename.svg"
        onTriggered: delegate.beginEdit();
    }

    Action {
        id: removeAction
        text: qsTr("!Delete...")
        icon.source: "qrc:/assets/gfx/symbolic/filemgr/action-remove.svg"

        onTriggered: {
            const doRemove = function() {
                Backend.fileManager.remove(delegate.fileName, delegate.isDirectory);
            };

            const msgObj = {
                title: "%1 \"%2\"?".arg(qsTr("Delete")).arg(delegate.fileName),
                message: qsTr("This action cannot be undone."),
                suggestedRole: ConfirmationDialog.RejectRole,
                customText: qsTr("Delete")
            };

            confirmationDialog.openWithMessage(doRemove, msgObj);
        }
    }

    function rightClick(mouse) {
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

    function doubleClick(mouse) {
        if(mouse.button === Qt.RightButton || !delegate.isDirectory) {
            return;
        }

        Backend.fileManager.cd(delegate.fileName)
    }

    function beginEdit() {
        nameEdit.text = nameLabel.text;
        nameEdit.selectAll();
        nameEdit.forceActiveFocus(Qt.MouseFocusReason);

        editBox.visible = true;
    }

    function commitEdit() {
        const oldName = delegate.fileName;
        const newName = nameEdit.text;

        if(delegate.isNewDirectory) {
            Backend.fileManager.commitMkDir(newName);
        } else if(oldName !== newName) {
            Backend.fileManager.rename(oldName, newName);
        }

        nameLabel.text = newName;
        editBox.visible = false;
    }
}
