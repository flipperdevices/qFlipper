import QtQuick 2.15

Item {
    id: control

    signal pressed
    signal released
    signal shortPress
    signal longPress
    signal repeat

    property int padding: 0
    property alias icon: button.icon
    property alias iconPath: button.iconPath
    property alias iconName: button.iconName

    width: button.implicitWidth + padding * 2
    height: button.implicitHeight + padding * 2

    function setPressed() {
        button.down = true;
        onButtonPressed();
    }

    function setReleased() {
        button.down = false;
        onButtonReleased();
    }

    function onButtonPressed() {
        control.pressed();

        if(!longTimer.running) {
            longTimer.start();
        }

        if(releaseTimer.running) {
           releaseTimer.stop();
        }
    }

    function onButtonReleased() {
        releaseTimer.start();

        if(longTimer.running) {
            longTimer.stop();
            control.shortPress();
        }

        if(repeatTimer.running) {
            repeatTimer.stop();
        }
    }

    onLongPress: {
        if(!repeatTimer.running) {
            repeatTimer.start();
        }
    }

    ImageButton {
        id: button
        anchors.centerIn: parent

        onPressed: onButtonPressed()
        onReleased: onButtonReleased()
    }

    Timer {
        id: releaseTimer
        repeat: false
        interval: 1
        onTriggered: control.released()
    }

    Timer {
        id: longTimer
        repeat: false
        interval: 350
        onTriggered: control.longPress()
    }

    Timer {
        id: repeatTimer
        repeat: true
        interval: 150
        onTriggered: control.repeat()
    }
}
