import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Dialog {
    id: dialog

    width: parent.width
    height: parent.height

    property string titleText
    property string contentText

    background: Rectangle {
        id: background
        color: "#111118"

        width: parent.width
        height: parent.height
    }

    header: RowLayout {
        Text {
            text: qsTr("Version ") + dialog.titleText
            color: "white"
            font.pointSize: 20
            font.bold: true

            Layout.topMargin: 16
            Layout.leftMargin: 16
        }
    }

    contentItem: ScrollView {
        clip: true

        Text {
            text: dialog.contentText
            textFormat: Text.MarkdownText
            color: "white"
        }
    }

    footer: RowLayout {
        StyledButton {
            text: qsTr("Close")
            onClicked: dialog.close()
            Layout.fillWidth: true
            Layout.margins: 16
            Layout.topMargin: 0
        }
    }
}
