import QtQuick 2.12
import QtQuick.Controls 2.12

Button {
    id: control
    enabled: text === "Update"

    background: Rectangle {
        id: buttonBg
        color: "white"

        implicitWidth: 100
        implicitHeight: 40

        radius: height/2

    }

    contentItem: Text {
        id: buttonText
        text: parent.text
        color: "black"

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        font.capitalization: Font.AllUppercase
        font.bold: true
    }

    states: [
        State {
            name: "down"
            when: control.down
            PropertyChanges {
                target: buttonBg
                color: "#888"
            }
        },

        State {
            name: "disabled"
            when: !control.enabled
            PropertyChanges {
                target: buttonBg
                color: "transparent"
            }

            PropertyChanges {
                target: buttonText
                color: "white"
            }
        }
    ]
}
