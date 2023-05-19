import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

    property string customText
    property alias message: messageLabel.text

    function openWithMessage(onClosedFunc, messageObj = {}) {
        control.title = messageObj.title;
        control.message = messageObj.message;
        control.customText = messageObj.customText;

        if(onClosedFunc) {
            const onDialogClosed = function() {
                control.closed.disconnect(onDialogClosed);
                onClosedFunc();
            }

            control.closed.connect(onDialogClosed);
        }

        control.open();
        widgetContents.forceActiveFocus();
    }

    contentWidget: Item {
        id: widgetContents
        implicitWidth: 430
        implicitHeight: layout.implicitHeight
        Keys.onPressed: function(event) {
            if(event.key === Qt.Key_Return) {
                control.close();
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

            SmallButton {
                radius: 7
                highlighted: true
                text: customText ? customText : qsTr("Ok")

                Layout.margins: 24
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 160
                Layout.preferredHeight: 42

                onClicked: control.close()
            }
        }
    }
}
