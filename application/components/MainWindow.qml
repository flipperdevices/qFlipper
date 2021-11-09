import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtGraphicalEffects 1.15

import Theme 1.0

Item {
    id: mainWindow

    signal expandStarted
    signal expandFinished
    signal collapseStarted
    signal collapseFinished

    readonly property int baseWidth: 830
    readonly property int baseHeight: 500

    readonly property int shadowSize: 16
    readonly property int shadowOffset: 4

    readonly property var device: deviceRegistry.currentDevice
    readonly property var deviceState: device ? device.state : undefined
    readonly property var deviceInfo: deviceState ? deviceState.info : undefined

    property alias controls: windowControls

    width: baseWidth
    height: baseHeight

    x: shadowSize
    y: shadowSize - shadowOffset

    PropertyAnimation {
        id: logExpand

        duration: 500
        target: mainWindow
        property: "height"

        from: target.baseHeight
        to: target.baseHeight * 2
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

        from: logExpand.to
        to: logExpand.from

        onStarted: mainWindow.collapseStarted()
        onFinished: mainWindow.collapseFinished()
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
            color: Qt.rgba(0, 0, 0, 0.3)
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

        Text {
            id: versionLabel

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            anchors.rightMargin: 16

            color: Theme.color.orange
            opacity: 0.5

            font.family: "Terminus (TTF)"
            font.pixelSize: 12

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

        NoDeviceOverlay {
            id: noDeviceOverlay
            anchors.fill: parent
            opacity: device ? 0 : 1
        }

        HomeOverlay {
            id: homeOverlay
            backgroundRect: bg
            anchors.fill: parent
            opacity: deviceState && !deviceState.isPersistent ? 1 : 0
        }

        UpdateOverlay {
            id: updateOverlay
            backgroundRect: bg
            anchors.fill: parent
            opacity: deviceState && deviceState.isPersistent ? 1 : 0
        }

        Image {
            id: deviceWidget

            x: deviceState && !deviceState.isPersistent ? Math.floor(mainContent.width / 2) : 216
            y: 85

            source: "qrc:/assets/gfx/images/flipper.svg"
            sourceSize: Qt.size(360, 156)

            Behavior on x {
                PropertyAnimation {
                    easing.type: Easing.InOutQuad
                    duration: 350
                }
            }
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

        Rectangle {
            id: popupBg
            color: Theme.color.darkorange1

            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                spacing: 10

                IconImage {
                    width: 22
                    height: 22

                    color: Theme.color.orange
                    source: "qrc:/assets/gfx/symbolic/info-small.svg"
                }

                Text {
                    id: popupText
                    text: !deviceState ? "Waiting for devices ..." : deviceState.isPersistent? "Do not unplug the device ..." : "Ready."

                    color: Theme.color.orange

                    font.capitalization: Font.AllUppercase
                    verticalAlignment: Text.AlignVCenter
                    Layout.fillWidth: true
                }
            }
        }
    }

    ScrollView {
        anchors.top: footerLayour.bottom
        anchors.left: mainContent.left
        anchors.right: mainContent.right
        anchors.bottom: parent.bottom

        anchors.topMargin: 14
        anchors.bottomMargin: 12

        visible: height > 0

        TextArea {
            id: logText
            padding: 0
            anchors.fill: parent
            color: Theme.color.orange
            text: "qFlipper version 0.5.3 commit deadba0bab."
        }
    }
}
