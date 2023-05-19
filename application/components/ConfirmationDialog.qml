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
    property int suggestedRole
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

        control.title = messageObj.title ? messageObj.title : "";
        control.message = messageObj.message ? messageObj.message : "";
        control.suggestedRole = messageObj.suggestedRole ?  messageObj.suggestedRole : ConfirmationDialog.AcceptRole
        control.customText = messageObj.customText ? messageObj.customText : ""

        control.rejected.connect(onDialogRejected);
        control.accepted.connect(onDialogAccepted);
        control.open();
        widgetContents.forceActiveFocus();
    }

    contentWidget: Item {
        id: widgetContents
        implicitWidth: 430
        implicitHeight: layout.implicitHeight
        KeyNavigation.tab: contentWidget
        Keys.onPressed: function(event) {
            if(event.key === Qt.Key_Tab) {
                 if (control.suggestedRole === ConfirmationDialog.RejectRole) {
                    control.suggestedRole = ConfirmationDialog.AcceptRole
                 } else if (control.suggestedRole === ConfirmationDialog.AcceptRole) {
                    control.suggestedRole = ConfirmationDialog.RejectRole
                 }
            } else if (event.key === Qt.Key_Return) {
                if (control.suggestedRole === ConfirmationDialog.RejectRole) {
                    control.rejected();
                } else if (control.suggestedRole === ConfirmationDialog.AcceptRole) {
                    control.accepted();
                }
            }
        }

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

            // TODO: Find a way to use a DialogButtonBox properly

            RowLayout {
                spacing: 30
                Layout.margins: 20
                Layout.fillWidth: true
                Layout.preferredHeight: 42
                layoutDirection: Qt.platform.os === "osx" ? Qt.RightToLeft : Qt.LeftToRight

                SmallButton {
                    radius: 7
                    text: customText.length ? customText : qsTr("Ok")
                    highlighted: control.suggestedRole === ConfirmationDialog.AcceptRole
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: control.accepted()
                }

                SmallButton {
                    radius: 7
                    text: qsTr("Cancel")
                    highlighted: control.suggestedRole === ConfirmationDialog.RejectRole
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: control.rejected()
                }
            }
        }
    }
}
