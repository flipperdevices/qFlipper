import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Dialog {
    id: control
    modal: true
    anchors.centerIn: parent

    property string subtitle

    background: Rectangle {
        id: background
        color: "#111118"

        width: control.width
        height: control.height

        border.color: "white"
        border.width: 1
        radius: 6
    }

    header: Item {}

    contentItem: ColumnLayout {
        id: contentLayout
        spacing: 40

        ColumnLayout {
            id: textLayout
            Layout.alignment: Qt.AlignHCenter

            Text {
                text: control.title
                color: "white"
                font.bold: true
                font.pixelSize: 22
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                text: control.subtitle
                color: "white"
                font.pixelSize: 18
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    footer: RowLayout {
        RowLayout {
            spacing: 20
            Layout.alignment: Qt.AlignHCenter
            Layout.margins: 20
            Layout.topMargin: 0

            StyledButton {
                id: yesButton
                text: qsTr("Yes")
                suggested: true
                Layout.fillWidth: true
            }

            StyledButton {
                id: noButton
                text: qsTr("No")
                Layout.fillWidth: true
            }
        }
    }

    Component.onCompleted: {
        yesButton.clicked.connect(control.accept)
        noButton.clicked.connect(control.reject)
    }
}
