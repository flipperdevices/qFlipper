import QtQuick 2.15
import QtQuick.Controls 2.15

import Theme 1.0
import Primitives 1.0

Popup {
    id: control

    signal accepted
    signal rejected

    padding: 0

    width: parent.width
    height: parent.height

    property alias radius: shadow.radius
    property alias title: titleLabel.text

    property bool closable: false
    property bool invertTitle: true
    property bool result: false
    property Item contentWidget: Item {}

    enter: Transition {
        PropertyAnimation {
            property: "opacity"; duration: 150; easing.type: Easing.InOutQuad; from: 0; to: 1
        }
    }

    exit: Transition {
        PropertyAnimation {
            property: "opacity"; duration: 150; easing.type: Easing.InOutQuad; from: 1; to: 0
        }
    }

    background: Rectangle {
        id: shadow
        color: "black"
        opacity: 0.8
    }

    contentItem: Item {
        Rectangle {
            id: blackBorder
            anchors.fill: contentBg
            anchors.margins: -3

            opacity: 0.5
            color: "black"

            radius: contentBg.radius - anchors.margins
        }

        Rectangle {
            id: contentBg

            width: Math.max(contentWidget.implicitWidth, titleLabel.implicitWidth + closable ? closeButton.width : 0) + border.width * 2
            height: header.height + contentWidget.implicitHeight + border.width * 2

            radius: 7
            border.width: 2

            anchors.centerIn: parent

            color: "black"
            border.color: Theme.color.lightorange2

            Rectangle {
                id: header
                height: 42
                x: contentBg.border.width
                y: contentBg.border.width
                width: contentBg.width - contentBg.border.width * 2
                radius: invertTitle ? 0 : contentBg.radius - contentBg.border.width
                color: invertTitle ? Theme.color.lightorange2 : Theme.color.darkorange1

                Rectangle {
                    color: parent.color
                    width: parent.width
                    height: parent.radius
                    y: parent.height - height
                    visible: parent.radius > 0
                }

                TextLabel {
                    id: titleLabel

                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8

                    color: invertTitle ? Theme.color.darkorange1 : Theme.color.lightorange2

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    elide: Text.ElideMiddle

                    Item {
                        id: closeButton
                        visible: closable
                        height: parent.height
                        width: height
                        anchors.right: parent.right

                        Button {
                            flat: true
                            padding: 0

                            width: 24
                            height: 24

                            backgroundColor: ColorSet {
                                normal: Theme.color.darkorange1
                                hover: Theme.color.mediumorange2
                                down: Theme.color.lightred2
                            }

                            anchors.centerIn: parent
                            icon.source: "qrc:/assets/gfx/controls/windows/close.svg"
                            icon.width: 20
                            icon.height: 20
                            onClicked: control.close()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        accepted.connect(close);
        rejected.connect(close);

        contentWidget.parent = contentBg;
        contentWidget.x = header.x;
        contentWidget.y = header.y + header.height;
    }

    onAccepted: {
        result = true;
    }

    onRejected: {
        result = false;
    }
}
