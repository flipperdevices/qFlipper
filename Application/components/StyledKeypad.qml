import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

RowLayout {
    id: control
    height: 200
    spacing: 0

    property real buttonRatio: 3.33
    property real buttonIconSize: 48

    Rectangle {
        id: mainPad

        color: "#222"
        border.width: 1
        border.color: "#2E2E2E"

        height: control.height
        width: height

        radius: height / 2

        StyledRoundButton {
            id: buttonPadMiddle
            color: "#444"
            border.color: "#181818"

            anchors.centerIn: parent
            width: Math.floor(control.height / buttonRatio)
        }

        Item {
            anchors.fill: parent
            anchors.margins: 12

            StyledToolButton {
                id: buttonPadUp
                width: control.buttonIconSize

                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter

                icon.source: "qrc:/assets/arrow-up.svg"
            }

            StyledToolButton {
                id: buttonPadDown
                width: control.buttonIconSize

                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                icon.source: "qrc:/assets/arrow-down.svg"
            }

            StyledToolButton {
                id: buttonPadLeft
                width: control.buttonIconSize

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                icon.source: "qrc:/assets/arrow-left.svg"
            }

            StyledToolButton {
                id: buttonPadRight
                width: control.buttonIconSize

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                icon.source: "qrc:/assets/arrow-right.svg"
            }
        }
    }

    StyledRoundButton {
        id: buttonPadBack

        width: Math.floor(control.height / buttonRatio)
        icon: "qrc:/assets/symbol-back.svg"

        Layout.alignment: Qt.AlignBottom
    }
}
