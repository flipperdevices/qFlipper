import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: element

    required property string version
    required property string timestamp
    required property var file

    signal installRequested(file: var)

    height: 80
    width: parent.width

    color: "black"
    border.color: "white"
    border.width: 1
    radius: 4

    RowLayout {
        anchors.fill: parent
        anchors.margins: 15

        Text {
            id: versionText
            text: version

            color: "white"
            font.capitalization: Font.AllUppercase
            font.pointSize: 14
            font.bold: true
            elide: Text.ElideLeft

            Layout.maximumWidth: 150
        }

        Text {
            id: timestampText
            text: timestamp

            color: "darkgray"
            font.capitalization: Font.AllUppercase
            font.pointSize: 12

            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 12
            Layout.alignment: Qt.AlignRight

            StyledButton {
                id: installButton
                text: qsTr("Install")
                suggested: true

                onClicked: {
                    installRequested(file);
                }
            }

            StyledButton {
                id: changelogButton
                text: qsTr("Changelog")
            }
        }
    }
}
