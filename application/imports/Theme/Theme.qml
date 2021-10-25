pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property var color: QtObject {
        readonly property color orange: "#fe8a2c"
        readonly property color darkorange: "#76380b"
        readonly property color lightorange: "#ff9722"

        readonly property color green: "#35cf3e"
        readonly property color darkgreen: "#0f3211"
        readonly property color lightgreen: "#67ff70"

        readonly property color brown: "#491d00"
        readonly property color darkbrown: "#662c00"

        readonly property color cyan: "#35cbdf"
        readonly property color red: "#ff5b27"
    }

}
