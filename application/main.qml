import QtQml 2.12
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls.Universal 2.12

import "./screens"

Window {
    id: root
    title: Qt.application.name

    width: 800
    height: 480
    visible: true

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

        onHomeRequested: mainLoader.setSource("qrc:/screens/homescreen.qml");
        onVersionsRequested: mainLoader.setSource("qrc:/screens/versionscreen.qml", {device: device});
        onStreamRequested: mainLoader.setSource("qrc:/screens/streamscreen.qml", {device: device})
    }
}
