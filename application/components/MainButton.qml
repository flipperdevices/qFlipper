import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0

Button {
    id: control

    enum Accent {
        Default,
        Green,
        Blue
    }

    property int accent: MainButton.Default

    width: 280
    height: 56

    radius: 7
    borderWidth: 3

    font.family: "Born2bSportyV2"
    font.pixelSize: 48

    states: [
        State {
            when: accent === MainButton.Green

            PropertyChanges {
                target: foregroundColor
                normal: Theme.color.lightgreen
                hover: Theme.color.lightgreen
                down: Theme.color.darkgreen
            }

            PropertyChanges {
                target: backgroundColor
                normal: Theme.color.mediumgreen2
                hover: Theme.color.mediumgreen1
                down: Theme.color.lightgreen
            }

            PropertyChanges {
                target: strokeColor
                normal: Theme.color.lightgreen
                hover: Theme.color.lightgreen
                down: Theme.color.lightgreen
            }
        },

        State {
            when: accent === MainButton.Blue

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
        }
    ]
}
