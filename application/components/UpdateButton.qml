import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

import "../style"

Button {
    id: control

    enum Accent {
        Default,
        Green,
        Blue
    }

    property int accent: UpdateButton.Default

    width: 280
    height: 56

    radius: 7
    borderWidth: 3

    text: qsTr("No updates")
    font.family: "Born2bSportyV2"
    font.pixelSize: 48

    states: [
        State {
            when: accent === UpdateButton.Green

            PropertyChanges {
                target: foregroundColor
                normal: Theme.color.lightgreen
                hover: Theme.color.lightgreen
                down: Theme.color.darkgreen
            }

            PropertyChanges {
                target: backgroundColor
                normal: Theme.color.green
                hover: Theme.color.mediumgreen
                down: Theme.color.lightgreen
            }

            PropertyChanges {
                target: strokeColor
                normal: Theme.color.lightgreen
                hover: Theme.color.lightgreen
                down: Theme.color.lightgreen
            }

            PropertyChanges {
                target: control
                text: qsTr("Update")
            }
        },

        State {
            when: accent === UpdateButton.Blue

            PropertyChanges {
                target: foregroundColor
                normal: Theme.color.lightblue
                hover: Theme.color.lightblue
                down: Theme.color.darkblue1
            }

            PropertyChanges {
                target: backgroundColor
                normal: Theme.color.darkblue2
                hover: Theme.color.darkblue1
                down: Theme.color.lightblue
            }

            PropertyChanges {
                target: strokeColor
                normal: Theme.color.lightblue
                hover: Theme.color.lightblue
                down: Theme.color.lightblue
            }

            PropertyChanges {
                target: control
                text: qsTr("Repair")
            }
        }
    ]
}
