import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

    property alias title: titleLabel.text
    property alias message: messageLabel.text
    property alias description: descriptionLabel.text

    function openWithMessage(onAcceptedFunc, messageObj = {}) {
        const onDialogRejected = function() {
            control.rejected.disconnect(onDialogRejected);
            control.accepted.disconnect(onDialogAccepted);
        }

        const onDialogAccepted = function() {
            onDialogRejected();
            onAcceptedFunc();
        }

        control.title = messageObj.title ? messageObj.title : qsTr("Proceed?");
        control.message = messageObj.message ? messageObj.message : qsTr("Operation message");
        control.description = messageObj.description ? messageObj.description : qsTr("Operation short description");

        control.rejected.connect(onDialogRejected);
        control.accepted.connect(onDialogAccepted);
        control.open();
    }

    contentItem: Item {
        Rectangle {
            anchors.fill: background
            anchors.margins: -3
            color: Theme.color.mediumorange3
            radius: background.radius - anchors.margins
        }

        Rectangle {
            id: background
            implicitWidth: 400
            implicitHeight: layout.implicitHeight
            anchors.centerIn: parent

            radius: 7
            border.width: 2

            color: "black"
            border.color: Theme.color.lightorange2

            ColumnLayout {
                id: layout
                anchors.fill: parent

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48

                    color: Theme.color.lightorange2
                    radius: background.radius

                    Rectangle {
                        width: parent.width
                        height: parent.radius
                        anchors.bottom: parent.bottom
                        color: parent.color
                    }

                    TextLabel {
                        id: titleLabel
                        anchors.fill: parent
                        color: Theme.color.darkorange1
                        font.family: "Born2bSportyV2"

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                TextLabel {
                    id: messageLabel
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.Wrap

                    padding: 6
                    bottomPadding: 0

                    Layout.fillWidth: true
                }

                TextLabel {
                    id: descriptionLabel
                    color: Theme.color.mediumorange1
                    wrapMode: Text.Wrap

                    padding: 6

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    Layout.fillWidth: true
                }

                Item {
                    Layout.fillHeight: true
                }

                DialogButtonBox {
                    topPadding: 0
                    Layout.fillWidth: true
                    standardButtons: Dialog.Ok | Dialog.Cancel

                    delegate: SmallButton {
                        id: delegate
                        implicitHeight: 42
                        font.family: "Born2bSportyV2"
                        highlighted: DialogButtonBox.buttonRole === DialogButtonBox.AcceptRole
                    }

                    onAccepted: control.accepted()
                    onRejected: control.rejected()
                }
            }
        }
    }
}
