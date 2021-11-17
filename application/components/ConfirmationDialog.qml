import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

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

    contentWidget: Item {
        implicitWidth: 400
        implicitHeight: layout.implicitHeight

        ColumnLayout {
            id: layout
            width: parent.implicitWidth

            Item {
                Layout.preferredHeight: 10
            }

            TextLabel {
                id: messageLabel
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap

                padding: 6
                bottomPadding: 0

                Layout.fillWidth: true
                Layout.bottomMargin: 6
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
                Layout.preferredHeight: 10
            }

            DialogButtonBox {
                topPadding: 0
                Layout.fillWidth: true
                standardButtons: Dialog.Ok | Dialog.Cancel

                delegate: SmallButton {
                    id: delegate
                    font.bold: true
                    implicitHeight: 42
                    highlighted: DialogButtonBox.buttonRole === DialogButtonBox.AcceptRole
                }

                onAccepted: control.accepted()
                onRejected: control.rejected()
            }
        }
    }
}
