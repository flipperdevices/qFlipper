import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

RowLayout {
    id: control
    height: 200
    spacing: 0

    property real buttonRatio: 3.33
    property real buttonIconSize: 48

    signal inputEvent(var key, var type)

    enum InputKey {
        Up,
        Down,
        Right,
        Left,
        Ok,
        Back
    }

    enum InputType {
        Press,
        Release,
        Short,
        Long,
        Repeat
    }

    Rectangle {
        id: mainPad

        color: "#222"
        border.width: 1
        border.color: "#2E2E2E"

        height: control.height
        width: height

        radius: height / 2

        StyledRoundButton {
            id: buttonPadOk
            color: "#444"
            border.color: "#181818"

            anchors.centerIn: parent
            width: Math.floor(control.height / buttonRatio)

            onPressed: inputEvent(StyledKeypad.InputKey.Ok, StyledKeypad.InputType.Press)
            onReleased: inputEvent(StyledKeypad.InputKey.Ok, StyledKeypad.InputType.Release)
            onClicked: inputEvent(StyledKeypad.InputKey.Ok, StyledKeypad.InputType.Short)
            onPressAndHold: inputEvent(StyledKeypad.InputKey.Ok, StyledKeypad.InputType.Long)
            onRepeat: inputEvent(StyledKeypad.InputKey.Ok, StyledKeypad.InputType.Repeat)
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

                onPressed: inputEvent(StyledKeypad.InputKey.Up, StyledKeypad.InputType.Press)
                onReleased: inputEvent(StyledKeypad.InputKey.Up, StyledKeypad.InputType.Release)
                onClicked: inputEvent(StyledKeypad.InputKey.Up, StyledKeypad.InputType.Short)
                onPressAndHold: inputEvent(StyledKeypad.InputKey.Up, StyledKeypad.InputType.Long)
                onRepeat: inputEvent(StyledKeypad.InputKey.Up, StyledKeypad.InputType.Repeat)
            }

            StyledToolButton {
                id: buttonPadDown
                width: control.buttonIconSize

                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                icon.source: "qrc:/assets/arrow-down.svg"

                onPressed: inputEvent(StyledKeypad.InputKey.Down, StyledKeypad.InputType.Press)
                onReleased: inputEvent(StyledKeypad.InputKey.Down, StyledKeypad.InputType.Release)
                onClicked: inputEvent(StyledKeypad.InputKey.Down, StyledKeypad.InputType.Short)
                onPressAndHold: inputEvent(StyledKeypad.InputKey.Down, StyledKeypad.InputType.Long)
                onRepeat: inputEvent(StyledKeypad.InputKey.Down, StyledKeypad.InputType.Repeat)
            }

            StyledToolButton {
                id: buttonPadLeft
                width: control.buttonIconSize

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                icon.source: "qrc:/assets/arrow-left.svg"

                onPressed: inputEvent(StyledKeypad.InputKey.Left, StyledKeypad.InputType.Press)
                onReleased: inputEvent(StyledKeypad.InputKey.Left, StyledKeypad.InputType.Release)
                onClicked: inputEvent(StyledKeypad.InputKey.Left, StyledKeypad.InputType.Short)
                onPressAndHold: inputEvent(StyledKeypad.InputKey.Left, StyledKeypad.InputType.Long)
                onRepeat: inputEvent(StyledKeypad.InputKey.Left, StyledKeypad.InputType.Repeat)
            }

            StyledToolButton {
                id: buttonPadRight
                width: control.buttonIconSize

                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                icon.source: "qrc:/assets/arrow-right.svg"

                onPressed: inputEvent(StyledKeypad.InputKey.Right, StyledKeypad.InputType.Press)
                onReleased: inputEvent(StyledKeypad.InputKey.Right, StyledKeypad.InputType.Release)
                onClicked: inputEvent(StyledKeypad.InputKey.Right, StyledKeypad.InputType.Short)
                onPressAndHold: inputEvent(StyledKeypad.InputKey.Right, StyledKeypad.InputType.Long)
                onRepeat: inputEvent(StyledKeypad.InputKey.Right, StyledKeypad.InputType.Repeat)
            }
        }
    }

    StyledRoundButton {
        id: buttonPadBack

        width: Math.floor(control.height / buttonRatio)
        icon: "qrc:/assets/symbol-back.svg"

        Layout.alignment: Qt.AlignBottom

        onPressed: inputEvent(StyledKeypad.InputKey.Back, StyledKeypad.InputType.Press)
        onReleased: inputEvent(StyledKeypad.InputKey.Back, StyledKeypad.InputType.Release)
        onClicked: inputEvent(StyledKeypad.InputKey.Back, StyledKeypad.InputType.Short)
        onPressAndHold: inputEvent(StyledKeypad.InputKey.Back, StyledKeypad.InputType.Long)
        onRepeat: inputEvent(StyledKeypad.InputKey.Back, StyledKeypad.InputType.Repeat)
    }
}
