import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

CustomDialog {
    id: control

    title: qsTr("SD Card Not Found")

    property bool isAltPressed: false

    contentWidget: RowLayout {
        spacing: -8

        Image {
            Layout.margins: 20
            source: "qrc:/assets/gfx/images/warning-no-sdcard.svg"
            sourceSize: Qt.size(118, 148)
        }

        ColumnLayout {
            spacing: 20
            Layout.margins: 20

            TextLabel {
                text: qsTr("Please install an <font color=\"%1\">SD Card</font><br>before updating the firmware")
                    .arg(Theme.color.lightred4)
                lineHeight: 1.35
                horizontalAlignment: Text.AlignRight
            }

            Button {
                id: okButton
                text: control.isAltPressed ? qsTr("Force install") : "OK"

                Keys.onPressed: control.isAltPressed = event.modifiers & Qt.AltModifier;
                Keys.onReleased: control.isAltPressed = false;

                onClicked: control.isAltPressed ? control.accepted() : control.rejected()

                Layout.preferredWidth: 200
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    onOpened: {
        okButton.forceActiveFocus();
    }

    onClosed: {
        isAltPressed = false;
    }
}
