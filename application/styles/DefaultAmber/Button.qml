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
import Primitives 1.0

T.Button {
    id: control

    property var foregroundColor: ColorSet {
        normal: Theme.color.lightorange2
        hover: Theme.color.lightorange1
        down: Theme.color.darkorange1
        disabled: Theme.color.mediumorange1
    }

    property var backgroundColor: ColorSet {
        normal: Theme.color.darkorange1
        hover: Theme.color.mediumorange2
        down: Theme.color.lightorange2
        disabled: Theme.color.darkorange2
    }

    property var strokeColor: ColorSet {
        normal: Theme.color.lightorange2
        hover: Theme.color.lightorange1
        down: Theme.color.lightorange2
        disabled: Theme.color.mediumorange1
    }

    property alias radius: bg.radius
    property alias borderWidth: bg.border.width

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    horizontalPadding: padding + 2
    spacing: 6

    icon.color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                 control.hovered ? foregroundColor.hover : foregroundColor.normal

    font.pixelSize: 32
    font.family: "HaxrCorp 4089"
    font.capitalization: Font.AllUppercase
    font.underline: flat

    contentItem: IconLabelEx {
        id: content

        text: control.text
        font: control.font

        color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                control.hovered ? foregroundColor.hover : foregroundColor.normal

        icon.source: control.icon.source
        icon.sourceSize.width: control.icon.width
        icon.sourceSize.height: control.icon.height
        icon.color: control.icon.color

        padding: control.padding
    }

    background: Rectangle {
        id: bg

        implicitWidth: 100
        implicitHeight: 42

        radius: 5

        color: !control.enabled ? backgroundColor.disabled : control.down ? backgroundColor.down :
                control.hovered ? backgroundColor.hover : backgroundColor.normal
        border.color: !control.enabled ? strokeColor.disabled : control.down ? strokeColor.down :
                       control.hovered ? strokeColor.hover : strokeColor.normal
        border.width: control.flat ? 0 : 2

        Behavior on color {
            ColorAnimation {
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
    }
}

