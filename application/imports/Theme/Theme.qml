pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property var color: QtObject {
        readonly property color transparent: Qt.rgba(0, 0, 0, 0)
        readonly property color orange: "#fe8a2c"
        readonly property color lightorange: "#ff9722"
        readonly property color darkorange1: "#491d00"
        readonly property color darkorange2: "#3a1f10"
        readonly property color mediumorange1: "#9e5823"
        readonly property color mediumorange2: "#76380b"
        readonly property color mediumorange3: "#652f11"

        readonly property color green: "#203812"
        readonly property color darkgreen: "#0c160c"
        readonly property color lightgreen: "#2ed832"
        readonly property color mediumgreen: "#285b12"

        readonly property color cyan: "#35cbdf"
        readonly property color darkcyan: "#174a51"
        readonly property color lightcyan: "#56ebff"

        readonly property color lightred1: "#ff5b27"
        readonly property color lightred2: "#ff5924"
        readonly property color mediumred1: "#953618"
        readonly property color mediumred2: "#672715"
        readonly property color darkred: "#451a0e"

        readonly property color red: "#f05c2e"
        readonly property color brown: "#3a140d"
    }

    readonly property var timing: QtObject {
        readonly property int toolTipDelay: 500
    }
}
