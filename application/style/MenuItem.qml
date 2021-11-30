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

T.MenuItem {
    id: control

    property var foregroundColor: ColorGroup {
        normal: Theme.color.lightorange2
        hover: Theme.color.lightorange1
        down: Theme.color.darkorange1
        disabled: Theme.color.mediumorange1
    }

    property var backgroundColor: ColorGroup {
        normal: Theme.color.darkorange1
        hover: Theme.color.mediumorange2
        down: Theme.color.lightorange2
        disabled: Theme.color.darkorange2
    }

    property var strokeColor: ColorGroup {
        normal: Theme.color.lightorange2
        hover: Theme.color.lightorange1
        down: Theme.color.lightorange2
        disabled: Theme.color.darkorange2
    }

    property bool isFirst: false
    property bool isLast: false

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding) + 5
    padding: 6
    spacing: 6

    horizontalPadding: 16

    icon.width: 24
    icon.height: 24
    icon.color: control.palette.windowText

    font.pixelSize: 32
    font.family: "HaxrCorp 4089"
    font.capitalization: Font.AllUppercase

    contentItem: IconLabel {
        readonly property real arrowPadding: control.subMenu && control.arrow ? control.arrow.width + control.spacing : 0
        readonly property real indicatorPadding: control.checkable && control.indicator ? control.indicator.width + control.spacing : 0
        leftPadding: !control.mirrored ? indicatorPadding : arrowPadding
        rightPadding: control.mirrored ? indicatorPadding : arrowPadding

        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: Qt.AlignLeft

        icon: control.icon
        text: control.text
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

    background: AdvancedRectangle {
        implicitWidth: 200
        implicitHeight: 42
        x: 4
        y: 4
        width: control.width - 8

        color: !control.enabled ? backgroundColor.disabled : control.down ? backgroundColor.down :
                control.hovered ? backgroundColor.hover : backgroundColor.normal

        topRadius: control.ObjectModel.index === 0 ? 5 : 0
        bottomRadius: control.ObjectModel.index === control.ListView.view.model.count - 1 ? 5 : 0
    }
}
