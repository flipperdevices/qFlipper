import QtQuick 2.15

Item {
    id: control

    signal pressed
    signal released
    signal shortPress
    signal longPress
    signal repeat

    property int padding: 0
    // Tracks whether a press is in progress, so a press is never left without a release
    property bool isDown: false
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

    // Ends a press that will not get a normal release (focus lost, window deactivated, ...)
    function forceRelease() {
        if(!isDown) {
            return;
        }

        button.down = false;
        onButtonReleased();
    }

    function onButtonPressed() {
        if(isDown) {
            return;
        }

        isDown = true;
        control.pressed();

        if(!longTimer.running) {
            longTimer.start();
        }

        if(releaseTimer.running) {
           releaseTimer.stop();
        }
    }

    function onButtonReleased() {
        if(!isDown) {
            return;
        }

        isDown = false;
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
        // AbstractButton emits canceled() instead of released() when the mouse is
        // released outside the button or the grab is lost. Without this the
        // repeat timer kept running and the key stayed held on the Flipper.
        onCanceled: onButtonReleased()
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
