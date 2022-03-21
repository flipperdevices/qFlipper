pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property var color: QtObject {
        readonly property color transparent: Qt.rgba(0, 0, 0, 0)

        readonly property color lightorange1: "#ff9722"
        readonly property color lightorange2: "#fe8a2c"
        readonly property color lightorange3: "#ce5300"
        readonly property color darkorange1: "#491d00"
        readonly property color darkorange2: "#3a1f10"
        readonly property color mediumorange1: "#9e5823"
        readonly property color mediumorange2: "#76380b"
        readonly property color mediumorange3: "#662c00"
        readonly property color mediumorange4: "#9b450b"
        readonly property color mediumorange5: "#9e4a12"

        readonly property color lightgreen: "#2ed832"
        readonly property color mediumgreen1: "#285b12"
        readonly property color mediumgreen2: "#203812"
        readonly property color darkgreen: "#0c160c"

        readonly property color lightblue: "#228cff"
        readonly property color mediumblue: "#143c66"
        readonly property color darkblue1: "#11355c"
        readonly property color darkblue2: "#152b47"

        readonly property color lightred1: "#ff5b27"
        readonly property color lightred2: "#ff5924"
        readonly property color lightred3: "#ff1f00"
        readonly property color lightred4: "#ff3c00"
        readonly property color mediumred1: "#953618"
        readonly property color mediumred2: "#672715"
        readonly property color darkred1: "#451a0e"
        readonly property color darkred2: "#331400"
    }

    readonly property var timing: QtObject {
        readonly property int toolTipDelay: 500
    }
}
