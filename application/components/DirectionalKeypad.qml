import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Theme 1.0

Item {
    id: control

    signal inputEvent(var key, var type)

    property int spacing: 15

    width: 186
    height: 224

    Rectangle {
        width: 168
        height: width
        radius: width / 2
        color: Theme.color.darkorange1
        border.color: Theme.color.lightorange1
        border.width: 2

        GridLayout {
            anchors.centerIn: parent
            rowSpacing: columnSpacing
            columnSpacing: 0
            columns: 3

            KeypadButton {
                id: buttonUp
                Layout.columnSpan: 3
                Layout.alignment: Qt.AlignHCenter

                iconName: "triangle"
                iconPath: "qrc:/assets/gfx/controls/dpad"

                icon.width: 32
                icon.height: 28

                rotation: 180
                padding: control.spacing

                onPressed: inputEvent(DirectionalKeypad.InputKey.Up, DirectionalKeypad.InputType.Press)
                onReleased: inputEvent(DirectionalKeypad.InputKey.Up, DirectionalKeypad.InputType.Release)
                onShortPress: inputEvent(DirectionalKeypad.InputKey.Up, DirectionalKeypad.InputType.Short)
                onLongPress: inputEvent(DirectionalKeypad.InputKey.Up, DirectionalKeypad.InputType.Long)
                onRepeat: inputEvent(DirectionalKeypad.InputKey.Up, DirectionalKeypad.InputType.Repeat)
            }

            KeypadButton {
                id: buttonLeft
                iconName: "triangle"
                iconPath: "qrc:/assets/gfx/controls/dpad"

                icon.width: 32
                icon.height: 28

                rotation: 90
                padding: control.spacing

                onPressed: inputEvent(DirectionalKeypad.InputKey.Left, DirectionalKeypad.InputType.Press)
                onReleased: inputEvent(DirectionalKeypad.InputKey.Left, DirectionalKeypad.InputType.Release)
                onShortPress: inputEvent(DirectionalKeypad.InputKey.Left, DirectionalKeypad.InputType.Short)
                onLongPress: inputEvent(DirectionalKeypad.InputKey.Left, DirectionalKeypad.InputType.Long)
                onRepeat: inputEvent(DirectionalKeypad.InputKey.Left, DirectionalKeypad.InputType.Repeat)
            }

            KeypadButton {
                id: buttonOk
                iconName: "circle"
                iconPath: "qrc:/assets/gfx/controls/dpad"

                icon.width: 52
                icon.height: 52

                onPressed: inputEvent(DirectionalKeypad.InputKey.Ok, DirectionalKeypad.InputType.Press)
                onReleased: inputEvent(DirectionalKeypad.InputKey.Ok, DirectionalKeypad.InputType.Release)
                onShortPress: inputEvent(DirectionalKeypad.InputKey.Ok, DirectionalKeypad.InputType.Short)
                onLongPress: inputEvent(DirectionalKeypad.InputKey.Ok, DirectionalKeypad.InputType.Long)
                onRepeat: inputEvent(DirectionalKeypad.InputKey.Ok, DirectionalKeypad.InputType.Repeat)
            }

            KeypadButton {
                id: buttonRight
                iconName: "triangle"
                iconPath: "qrc:/assets/gfx/controls/dpad"

                icon.width: 32
                icon.height: 28

                rotation: -90
                padding: control.spacing

                onPressed: inputEvent(DirectionalKeypad.InputKey.Right, DirectionalKeypad.InputType.Press)
                onReleased: inputEvent(DirectionalKeypad.InputKey.Right, DirectionalKeypad.InputType.Release)
                onShortPress: inputEvent(DirectionalKeypad.InputKey.Right, DirectionalKeypad.InputType.Short)
                onLongPress: inputEvent(DirectionalKeypad.InputKey.Right, DirectionalKeypad.InputType.Long)
                onRepeat: inputEvent(DirectionalKeypad.InputKey.Right, DirectionalKeypad.InputType.Repeat)
            }

            KeypadButton {
                id: buttonDown
                Layout.columnSpan: 3
                Layout.alignment: Qt.AlignHCenter

                iconName: "triangle"
                iconPath: "qrc:/assets/gfx/controls/dpad"

                icon.width: 32
                icon.height: 28
                padding: control.spacing

                onPressed: inputEvent(DirectionalKeypad.InputKey.Down, DirectionalKeypad.InputType.Press)
                onReleased: inputEvent(DirectionalKeypad.InputKey.Down, DirectionalKeypad.InputType.Release)
                onShortPress: inputEvent(DirectionalKeypad.InputKey.Down, DirectionalKeypad.InputType.Short)
                onLongPress: inputEvent(DirectionalKeypad.InputKey.Down, DirectionalKeypad.InputType.Long)
                onRepeat: inputEvent(DirectionalKeypad.InputKey.Down, DirectionalKeypad.InputType.Repeat)
            }
        }
    }

    KeypadButton {
        id: buttonBack
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        iconName: "back"
        iconPath: "qrc:/assets/gfx/controls/dpad"

        icon.width: 52
        icon.height: 52

        onPressed: inputEvent(DirectionalKeypad.InputKey.Back, DirectionalKeypad.InputType.Press)
        onReleased: inputEvent(DirectionalKeypad.InputKey.Back, DirectionalKeypad.InputType.Release)
        onShortPress: inputEvent(DirectionalKeypad.InputKey.Back, DirectionalKeypad.InputType.Short)
        onLongPress: inputEvent(DirectionalKeypad.InputKey.Back, DirectionalKeypad.InputType.Long)
        onRepeat: inputEvent(DirectionalKeypad.InputKey.Back, DirectionalKeypad.InputType.Repeat)
    }

    Keys.onPressed: function(event) {
        if(event.isAutoRepeat)
            return;

        const button = findButton(event.key);

        if(button === null)
            return;

        button.setPressed();
        event.accepted = true;
    }

    Keys.onReleased: function(event) {
        if(event.isAutoRepeat)
            return;

        const button = findButton(event.key);

        if(button === null)
            return;

        button.setReleased();
        event.accepted = true;
    }

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

    function findButton(key) {
        switch(key) {
        case Qt.Key_Left:
        case Qt.Key_H:
        case Qt.Key_A:
            return buttonLeft;
        case Qt.Key_Right:
        case Qt.Key_L:
        case Qt.Key_D:
            return buttonRight;
        case Qt.Key_Up:
        case Qt.Key_K:
        case Qt.Key_W:
            return buttonUp;
        case Qt.Key_Down:
        case Qt.Key_J:
        case Qt.Key_S:
            return buttonDown;
        case Qt.Key_Enter:
        case Qt.Key_Return:
        case Qt.Key_Space:
        case Qt.Key_E:
        case Qt.Key_Z:
            return buttonOk;
        case Qt.Key_Q:
        case Qt.Key_X:
        case Qt.Key_Escape:
        case Qt.Key_Backspace:
            return buttonBack;
        default:
            return null;
        }
    }

}
