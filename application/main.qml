import QtQuick 2.15
import QtQuick.Window 2.15

import "components"

Window {
    id: root
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint
    title: Qt.application.displayName

    height: mainWindow.baseHeight + mainWindow.shadowSize * 2

    minimumWidth: mainWindow.baseWidth + mainWindow.shadowSize * 2
    minimumHeight: mainWindow.baseHeight + mainWindow.shadowSize * 2

    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    color: "transparent"

    DragHandler {
        onActiveChanged: if(active) { root.startSystemMove(); }
        target: null
    }

    MainWindow {
        id: mainWindow
        anchors.margins: shadowSize
        anchors.top: root.contentItem.top

        onExpandStarted: {
            root.maximumHeight = 16384;
            root.height = baseHeight + logHeight + shadowSize * 2;
        }

        onExpandFinished: {
            anchors.bottom = root.contentItem.bottom
        }

        onCollapseStarted: {
            anchors.bottom = undefined;
        }

        onCollapseFinished: {
            root.height = baseHeight + shadowSize * 2;
            root.maximumHeight = root.height;
        }
    }

    Component.onCompleted: {
        mainWindow.controls.minimizeRequested.connect(root.showMinimized);
        mainWindow.controls.closeRequested.connect(Qt.quit);
    }
}
