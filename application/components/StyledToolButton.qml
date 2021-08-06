import QtQml 2.12
import QtQuick 2.0
import QtQuick.Controls 2.12

ToolButton {
    id: control
    width: 50
    height: width

    icon.color: "white"
    icon.width: 32
    icon.height: 32

    signal repeat()

    onPressAndHold: {
        repeatTimer.start()
    }

    onReleased: {
        if(repeatTimer.running) {
            repeatTimer.stop();
        }
    }

    Timer {
        id: repeatTimer
        repeat: true
        interval: 100
        onTriggered: control.repeat()
    }
}
