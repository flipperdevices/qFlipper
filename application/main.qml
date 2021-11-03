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

//    Rectangle {
//        id: bounds
//        anchors.fill: parent
//        color: "transparent"
//        border.color: Qt.rgba(0, 1, 0, 0.3)
//        border.width: 1
//    }

    MainWindow {
        id: mainWindow

        onExpandStarted: {
            root.maximumHeight = baseHeight * 2 + shadowSize * 2;
            root.height = root.maximumHeight;
        }

        onExpandFinished: {
            root.minimumHeight = root.maximumHeight;
        }

        onCollapseStarted: {
            root.minimumHeight = baseHeight + shadowSize * 2;
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
