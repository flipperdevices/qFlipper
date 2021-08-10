import QtQml 2.12
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls.Universal 2.12

import "./screens"

Window {
    id: root
    title: Qt.application.displayName

    width: 800
    height: 480
    visible: true

    minimumWidth: 800
    minimumHeight: 480

    color: "black"
    Universal.theme: "Dark"

    Loader {
        id: mainLoader
        anchors.fill: parent
        source: "qrc:/screens/homescreen.qml"
    }

    Connections {
        target: mainLoader.item
        ignoreUnknownSignals: true

        function onHomeRequested() { mainLoader.setSource("qrc:/screens/homescreen.qml"); }
        function onVersionsRequested(device) { mainLoader.setSource("qrc:/screens/versionscreen.qml", {device: device}); }
        function onStreamRequested(device) { mainLoader.setSource("qrc:/screens/streamscreen.qml", {device: device}); }
    }
}
