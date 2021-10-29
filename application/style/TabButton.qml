/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T

import Theme 1.0

T.TabButton {
    id: control

    property var foregroundColor: ColorGroup {
        normal: Theme.color.orange
        hover: Theme.color.lightorange
        down: Theme.color.darkorange1
        disabled: Theme.color.mediumorange1
    }

    property var backgroundColor: ColorGroup {
        normal: Theme.color.darkorange1
        hover: Theme.color.mediumorange2
        down: Theme.color.orange
        disabled: Theme.color.darkorange2
    }

    property var strokeColor: ColorGroup {
        normal: Theme.color.orange
        hover: Theme.color.lightorange
        down: Theme.color.orange
        disabled: Theme.color.darkorange2
    }

    property alias radius: bg.radius
    property alias borderWidth: bg.border.width

    radius: 4
    borderWidth: 2

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)
    padding: 6
    spacing: 6

    icon.width: 24
    icon.height: 24
    icon.color: foregroundColor.normal

    ToolTip.delay: Theme.timing.toolTipDelay
    ToolTip.visible: hovered && ToolTip.text

    contentItem: IconLabel {
        id: content
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font

        color: foregroundColor.normal
    }

    background: Rectangle {
        id: bg

        implicitWidth: 47
        implicitHeight: 41

        color: backgroundColor.normal
        border.color: strokeColor.normal

        Item {
            clip: true
            height: control.radius

            anchors.bottom: bg.bottom
            anchors.right: bg.right
            anchors.left: bg.left

            Rectangle {
                height: parent.height + bg.border.width

                color: bg.color
                border.color: bg.border.color
                border.width: bg.border.width

                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
            }
        }

        Behavior on color {
            ColorAnimation {
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
    }

    states: [
        State {
            name: "down"
            when: control.down || control.checked

            PropertyChanges {
                target: bg
                color: backgroundColor.down
                border.color: strokeColor.down
            }

            PropertyChanges {
                target: content
                color: foregroundColor.down
                icon.color: foregroundColor.down
            }
        },

        State {
            name: "hovered"
            when: control.hovered

            PropertyChanges {
                target: bg
                color: backgroundColor.hover
                border.color: strokeColor.hover
            }

            PropertyChanges {
                target: content
                color: foregroundColor.hover
                icon.color: foregroundColor.hover
            }
        },

        State {
            name: "disabled"
            when: !control.enabled

            PropertyChanges {
                target: bg
                color: backgroundColor.disabled
                border.color: strokeColor.disabled
            }

            PropertyChanges {
                target: content
                color: foregroundColor.disabled
                icon.color: foregroundColor.disabled
            }
        }
    ]
}
