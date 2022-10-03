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
import QtQml.Models 2.15

import Theme 1.0
import Primitives 1.0
import Misc 1.0

T.MenuItem {
    id: control

    readonly property bool destructive : text.startsWith("!")

    property var foregroundColor: ColorSet {
        normal: destructive ? Theme.color.lightred4 : Theme.color.lightorange2
        hover: destructive ? Theme.color.lightred4 : Theme.color.lightorange1
        down: destructive ? Theme.color.darkred1 : Theme.color.darkorange1
        disabled: destructive ? Theme.color.mediumred1 : Theme.color.mediumorange1
    }

    property var backgroundColor: ColorSet {
        normal: Theme.color.darkorange1
        hover: destructive ? Theme.color.mediumred2 : Theme.color.mediumorange2
        down: destructive ? Theme.color.lightred4 : Theme.color.lightorange2
        disabled: Theme.color.transparent
    }

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding) + 5
    padding: 0
    spacing: 10

    horizontalPadding: 8

    topInset: 0
    bottomInset: 0

    icon.width: 16
    icon.height: 16
    icon.color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                 control.hovered ? foregroundColor.hover : foregroundColor.normal

    font.pixelSize: 16
    font.family: "Share Tech"

    contentItem: IconLabel {
        antialiasing: Mitigations.fontRenderingFix
        readonly property real arrowPadding: control.subMenu && control.arrow ? control.arrow.width + control.spacing : 0
        readonly property real indicatorPadding: control.checkable && control.indicator ? control.indicator.width + control.spacing : 0
        leftPadding: !control.mirrored ? indicatorPadding : arrowPadding
        rightPadding: control.mirrored ? indicatorPadding : arrowPadding

        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: Qt.AlignLeft

        icon: control.icon
        text: control.destructive ? control.text.substring(1) : control.text
        font: control.font
        color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                control.hovered ? foregroundColor.hover : foregroundColor.normal
    }

    indicator: ColorImage {
        x: control.mirrored ? control.width - width - control.rightPadding : control.leftPadding
        y: control.topPadding + (control.availableHeight - height) / 2

        visible: control.checked
        source: control.checkable ? "qrc:/qt-project.org/imports/QtQuick/Controls.2/images/check.png" : ""
        color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                control.hovered ? foregroundColor.hover : foregroundColor.normal
        defaultColor: foregroundColor.normal
    }

    arrow: ColorImage {
        x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2

        visible: control.subMenu
        mirror: control.mirrored
        source: control.subMenu ? "qrc:/qt-project.org/imports/QtQuick/Controls.2/images/arrow-indicator.png" : ""
        color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                control.hovered ? foregroundColor.hover : foregroundColor.normal
        defaultColor: foregroundColor.normal
    }

    background: Rectangle {
        x: 1
        y: 1

        implicitWidth: 150
        implicitHeight: 25

        width: control.width - 2
        color: !control.enabled ? backgroundColor.disabled : control.down ? backgroundColor.down :
                control.hovered ? backgroundColor.hover : backgroundColor.normal

        Behavior on color {
            ColorAnimation {
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
    }
}
