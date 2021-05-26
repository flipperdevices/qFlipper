import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Popup {
    id: control
    modal: true
    anchors.centerIn: parent

    property string title
    property string subtitle

    signal accepted()
    signal rejected()

    background: Rectangle {
        id: background
        color: "black"

        width: control.width
        height: control.height

        border.color: "white"
        border.width: 1
        radius: 6
    }

    contentItem: Item {
        id: content

        ColumnLayout {
            id: contentLayout
            anchors.centerIn: content
            spacing: 30

            ColumnLayout {
                id: textLayout

                Text {
                    text: control.title
                    color: "white"
                    font.pointSize: 14
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    text: control.subtitle
                    color: "darkgray"
                    font.pointSize: 10
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            RowLayout {
                spacing: 30
                Layout.alignment: Qt.AlignHCenter

                StyledButton {
                    id: yesButton
                    text: qsTr("Yes")
                    suggested: true
                }

                StyledButton {
                    id: noButton
                    text: qsTr("No")
                }
            }
        }

        Text {
            text: "?"
            color: "darkgray"
            font.pointSize: 48
            font.bold: true

            anchors.right: contentLayout.left
            anchors.top: contentLayout.top
            anchors.rightMargin: 50
            anchors.topMargin: -20
        }
    }

    Component.onCompleted: {
        yesButton.clicked.connect(control.accepted)
        noButton.clicked.connect(control.rejected)
        yesButton.clicked.connect(control.close)
        noButton.clicked.connect(control.close)
        control.closed.connect(control.rejected)
    }
}
