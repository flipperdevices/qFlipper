import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import Theme 1.0
import QFlipper 1.0

Item {
    id: mainWindow

    signal expandStarted
    signal expandFinished
    signal collapseStarted
    signal collapseFinished
    signal resizeStarted
    signal resizeFinished

    property alias controls: windowControls

    readonly property int baseWidth: 830
    readonly property int baseHeight: 500

    readonly property int logHeight: 200
    readonly property int minimumLogHeight: 200

    readonly property int shadowSize: 16
    readonly property int shadowOffset: 4

    readonly property var device: Backend.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

//    Component.onCompleted: {
//        if(applicationUpdates.isReady) {
//            askForSelfUpdate();
//        } else {
//            applicationUpdates.channelsChanged.connect(askForSelfUpdate);
//            homeOverlay.selfUpdateRequested.connect(askForSelfUpdate);
//        }
//    }

    width: baseWidth
    height: baseHeight

    x: shadowSize
    y: shadowSize - shadowOffset

    PropertyAnimation {
        id: logExpand

        duration: 500
        target: mainWindow
        property: "height"

        to: target.baseHeight + logHeight
        easing.type: Easing.InOutQuad

        onStarted: mainWindow.expandStarted()
        onFinished: mainWindow.expandFinished()
    }

    PropertyAnimation {
        id: logCollapse
        target: logExpand.target
        easing: logExpand.easing
        duration: logExpand.duration
        property: logExpand.property

        to: target.baseHeight

        onStarted: mainWindow.collapseStarted()
        onFinished: mainWindow.collapseFinished()
    }

    ConfirmationDialog {
        id: confirmationDialog
        radius: bg.radius
        parent: bg
    }

    Rectangle {
        id: blackBorder
        anchors.fill: parent
        anchors.margins: -1
        radius: bg.radius + 1
        opacity: 0.5
        color: "black"
    }

    Rectangle {
        id: bg
        radius: 10
        anchors.fill: parent

        color: "black"
        border.color: Theme.color.mediumorange3
        border.width: 2

        layer.enabled: true
        layer.effect: DropShadow {
            radius: shadowSize
            samples: shadowSize * 2 + 1
            horizontalOffset: 0
            verticalOffset: shadowOffset
            color: Qt.rgba(0, 0, 0, 0.7)
        }
    }

    WindowControls {
        id: windowControls

        controlPath: "qrc:/assets/gfx/controls"

        anchors.top: mainWindow.top
        anchors.left: mainContent.left
        anchors.right: mainContent.right
        anchors.bottom: mainContent.top

        anchors.topMargin: bg.border.width
    }

    GridBackground {
        id: mainContent

        anchors.horizontalCenter: parent.horizontalCenter
        y: 38

        width: 800 + border.width * 2
        height: 390 + border.width * 2

        TextLabel {
            id: versionLabel

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            anchors.rightMargin: 16

            color: Theme.color.lightorange2
            opacity: 0.5

            font.family: "ProggySquareTT"
            font.pixelSize: 16

            text: app.version

            MouseArea {
                id: clicker
                property bool checked: false
                anchors.fill: parent

                onClicked: {
                    checked = !checked
                }
            }
        }

        DeviceWidget {
            id: deviceWidget
//            opacity: (windowState === MainWindow.Streaming) || (windowState === MainWindow.SelfUpdating) ? 0 : 1
            x: Backend.state === Backend.Ready ? Math.round(mainContent.width / 2) : 216
            y: 85

//            onScreenStreamRequested: streamOverlay.opacity = 1
        }

        NoDeviceOverlay {
            id: noDeviceOverlay
            anchors.fill: parent
            opacity: Backend.state === Backend.WaitingForDevices ? 1 : 0
        }

        HomeOverlay {
            id: homeOverlay
            backgroundRect: bg
            anchors.fill: parent
            opacity: Backend.state === Backend.Ready ? 1 : 0
        }

        UpdateOverlay {
            id: updateOverlay
            backgroundRect: bg
            anchors.fill: parent
            opacity: (Backend.state > Backend.Ready) &&
                     (Backend.state < Backend.Finished) ? 1 : 0
        }

        FinishOverlay {
            id: finishOverlay
            backgroundRect: bg
            anchors.fill: parent
            opacity: (Backend.state === Backend.Finished) ||
                     (Backend.state === Backend.ErrorOccured) ? 1 : 0
        }

        StreamOverlay {
            id: streamOverlay
            anchors.fill: parent
            opacity: 0
        }
    }

    RowLayout {
        id: footerLayour
        width: mainContent.width

        height: 42
        spacing: 15

        anchors.horizontalCenter: mainContent.horizontalCenter
        anchors.top: mainContent.bottom
        anchors.topMargin: 13

        Button {
            id: logButton
            text: qsTr("LOGS")

            Layout.preferredWidth: 110
            Layout.fillHeight: true

            icon.source: checked ? "qrc:/assets/gfx/symbolic/arrow-up.svg" :
                                   "qrc:/assets/gfx/symbolic/arrow-down.svg"
            icon.width: 24
            icon.height: 24

            checkable: true

            onCheckedChanged: {
                if(checked) {
                    if(!logCollapse.running) {
                        logExpand.start();
                    } else {
                        checked = false;
                    }

                } else {
                    if(!logExpand.running) {
                        logCollapse.start();
                    } else {
                        checked = true;
                    }
                }
            }
        }

        StatusBar {
            id: statusBar
            deviceState: mainWindow.deviceState

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    TextView {
        id: logView
        visible: height > 0

        anchors.top: footerLayour.bottom
        anchors.left: mainContent.left
        anchors.right: mainContent.right
        anchors.bottom: parent.bottom

        anchors.topMargin: 14
        anchors.bottomMargin: 12

        menu: Menu {
            id: logMenu
            width: 300

            MenuItem {
                text: "Select all"
                onTriggered: logView.content.selectAll()
            }

            MenuItem {
                text: "Copy to clipboard"
                onTriggered: logView.content.copy()
            }

            MenuItem {
                text: "Browse all logs..."
                onTriggered: Qt.openUrlExternally(Logger.logsPath)
            }
        }

        Component.onCompleted: {
            Logger.messageArrived.connect(logView.content.append);
            Logger.messageArrived.connect(logView.scrollToBottom);
        }
    }

    MouseArea {
        id: resizer

        property int prevMouseY

        width: parent.width
        height: 20

        visible: logView.visible && !logCollapse.running && !logExpand.running
        cursorShape: Qt.SizeVerCursor

        anchors.bottom: parent.bottom

        preventStealing: true

        onPressed: {
            prevMouseY = mouseY;
            mainWindow.resizeStarted();
        }

        onReleased: mainWindow.resizeFinished()

        onMouseYChanged: {
            const dy = mouseY - prevMouseY;
            mainWindow.height = Math.max(mainWindow.height + dy, mainWindow.baseHeight + mainWindow.minimumLogHeight);
        }
    }

//    function askForSelfUpdate() {
//        if(app.updater.canUpdate(applicationUpdates.latestVersion)) {
//            const messageObj = {
//                title : qsTr("Update qFlipper?"),
//                message: qsTr("Newer version of qFlipper<br/>will be installed"),
//                customText: qsTr("Update")
//            };

//            confirmationDialog.openWithMessage(function() {
//                app.updater.installUpdate(applicationUpdates.latestVersion);
//            }, messageObj);
//        }
//    }

}
