import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

    enum SuggestedButton {
        AcceptRole,
        RejectRole
    }

    property string customText
    property int suggestedRole: ConfirmationDialog.AcceptRole
    property alias message: messageLabel.text

    function openWithMessage(onAcceptedFunc, messageObj = {}) {
        const onDialogRejected = function() {
            control.rejected.disconnect(onDialogRejected);
            control.accepted.disconnect(onDialogAccepted);
        }

        const onDialogAccepted = function() {
            onDialogRejected();
            onAcceptedFunc();
        }

        control.title = messageObj.title ? messageObj.title : control.title;
        control.message = messageObj.message ? messageObj.message : control.message;
        control.suggestedRole = messageObj.suggestedRole ?  messageObj.suggestedRole :control.suggestedRole
        control.customText = messageObj.customText ? messageObj.customText : control.customText

        control.rejected.connect(onDialogRejected);
        control.accepted.connect(onDialogAccepted);
        control.open();
    }

    contentWidget: Item {
        implicitWidth: 430
        implicitHeight: layout.implicitHeight

        ColumnLayout {
            id: layout
            width: parent.implicitWidth

            TextLabel {
                id: messageLabel
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                lineHeight: 1.4
                wrapMode: Text.Wrap

                Layout.topMargin: 24
                Layout.bottomMargin: -8
                Layout.fillWidth: true
            }

            DialogButtonBox {
                Layout.fillWidth: true
                standardButtons: Dialog.Ok | Dialog.Cancel

                delegate: SmallButton {
                    id: delegate

                    implicitHeight: 42
                    // TODO: fix the button width
//                    text: customText && (DialogButtonBox.buttonRole === DialogButtonBox.AcceptRole) ? customText : text

                    highlighted: {
                        switch(control.suggestedRole) {
                        case ConfirmationDialog.AcceptRole:
                            return DialogButtonBox.buttonRole === DialogButtonBox.AcceptRole;
                        case ConfirmationDialog.RejectRole:
                            return DialogButtonBox.buttonRole === DialogButtonBox.RejectRole;
                        default:
                            return false;
                        }
                    }
                }

                onAccepted: control.accepted()
                onRejected: control.rejected()
            }
        }
    }
}
