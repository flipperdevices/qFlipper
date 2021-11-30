import QtQuick 2.15

Item {
    id: item
    property int radius: 0

    property alias topRadius: topBg.radius
    property alias bottomRadius: bottomBg.radius

    property alias border: middle.border
    property alias color: middle.color

    topRadius: radius
    bottomRadius: radius

    border.width: 0

    Rectangle {
        id: middle
        width: parent.width
        height: parent.height - top.height - bottom.height + 2 * border.width
        y: topRadius
    }

    Item {
        id: top
        clip: true
        width: parent.width
        height: topRadius + border.width

        Rectangle {
            id: topBg
            width: parent.width
            height: parent.height * 2
            border.color: item.border.color
            border.width: item.border.width
            color: item.color
        }
    }

    Item {
        id: bottom
        clip: true
        width: parent.width
        height: bottomRadius + border.width
        y: middle.y + middle.height - border.width

        Rectangle {
            id: bottomBg
            y: -parent.height
            width: parent.width
            height: parent.height * 2
            border.color: item.border.color
            border.width: item.border.width
            color: item.color
        }
    }
}
