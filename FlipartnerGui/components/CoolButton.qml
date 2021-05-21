import QtQuick 2.0
import QtQuick.Controls 2.12

Button {
    id: control

    background: Rectangle {
        id: buttonBg
        color: "#222"
        border.color: "#444"
        border.width: 2
        radius: height/2
        implicitWidth: 100
        implicitHeight: 40

        states: [
            State {
                name: "normal"
                when: !control.down
                PropertyChanges {
                    target: buttonBg
                }
            },

            State {
                name: "down"
                when: control.down
                PropertyChanges {
                    target: buttonBg
                    color: "#333"
                }
            }
        ]
    }

    contentItem: Text {
        text: parent.text
        color: "darkgray"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        font.capitalization: Font.AllUppercase
        font.bold: true
    }

}
