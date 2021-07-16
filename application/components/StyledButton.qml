import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12

Button {
    id: control

    property bool suggested: false
    property bool dangerous: false

    icon.width: 16
    icon.height: 16
    icon.color: control.enabled ? "white" : "#555"

    background: Rectangle {
        id: buttonBg
        color: "#222"
        border.color: "#2E2E2E"
        border.width: 1

        implicitWidth: 100
        implicitHeight: 40

        radius: 6

    }

    contentItem: IconLabel {
        id: buttonText
        text: parent.text
        color: "white"

        icon: control.icon
        display: control.display

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
                border.width: 0
            }

            PropertyChanges {
                target: buttonText
                color: "black"
            }

            PropertyChanges {
                target: control
                icon.color: "black"
            }
        },

        State {
            name: "suggested"
            when: control.suggested && control.enabled
            PropertyChanges {
                target: buttonBg
                color: "#5eba7d"
                border.width: 1
                border.color: "#6FA"
            }

            PropertyChanges {
                target: buttonText
                color: "black"
            }
        },

        State {
            name: "dangerous"
            when: control.dangerous
            PropertyChanges {
                target: buttonBg
                color: "#700"
                border.width: 1
                border.color: "#B00"
            }

            PropertyChanges {
                target: buttonText
                color: "white"
            }
        },

        State {
            name: "disabled"
            when: !control.enabled
            PropertyChanges {
                target: buttonBg
                color: "#33000000"
                border.color: "#22FFFFFF"
            }

            PropertyChanges {
                target: buttonText
                color: "white"
            }
        }
    ]
}
