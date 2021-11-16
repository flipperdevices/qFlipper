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
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T

import Theme 1.0

T.ComboBox {
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

    property alias radius: bg.radius
    property alias borderWidth: bg.border.width

    font.capitalization: Font.AllUppercase

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    leftPadding: padding + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding: padding + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)

    radius: 5

    delegate: ItemDelegate {
        width: ListView.view.width
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData

        palette.text: foregroundColor.normal
        palette.highlightedText: down ? foregroundColor.down : foregroundColor.hover

        palette.light: backgroundColor.hover
        palette.midlight: backgroundColor.down

        font: control.font

        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

    indicator: ColorImage {
        id: icon

        x: control.mirrored ? control.padding : control.width - width - 13
        y: control.topPadding + (control.availableHeight - height) / 2

        color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                control.hovered ? foregroundColor.hover : foregroundColor.normal
        source: control.down ? "qrc:/assets/gfx/symbolic/arrow-up.svg" :
                               "qrc:/assets/gfx/symbolic/arrow-down.svg"
        sourceSize: Qt.size(24, 24)
    }

    contentItem: T.TextField {
        id: content
        leftPadding: !control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
        rightPadding: control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
        topPadding: 6 - control.padding
        bottomPadding: 6 - control.padding

        text: control.editable ? control.editText : control.displayText

        enabled: control.editable
        autoScroll: control.editable
        readOnly: control.down
        inputMethodHints: control.inputMethodHints
        validator: control.validator
        selectByMouse: control.selectTextByMouse

        font: control.font

        color: !control.enabled ? foregroundColor.disabled : control.down ? foregroundColor.down :
                control.hovered ? foregroundColor.hover : foregroundColor.normal

        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
        verticalAlignment: Text.AlignVCenter

        background: Rectangle {
            visible: control.enabled && control.editable && !control.flat
            border.width: parent && parent.activeFocus ? 2 : 1
            border.color: parent && parent.activeFocus ? control.palette.highlight : control.palette.button
            color: control.palette.base
        }
    }

    background: Rectangle {
        id: bg

        implicitWidth: 140
        implicitHeight: 42

        color: !control.enabled ? backgroundColor.disabled : control.down ? backgroundColor.down :
                control.hovered ? backgroundColor.hover : backgroundColor.normal
        border.color: !control.enabled ? strokeColor.disabled : control.down ? strokeColor.down :
                       control.hovered ? strokeColor.hover : strokeColor.normal
        border.width: 2

        Rectangle {
            y: control.popup.y < 0 ? 0 : parent.height - height

            width: parent.width
            height: parent.radius

            color: backgroundColor.down
            visible: control.popup.visible
        }

        Behavior on color {
            ColorAnimation {
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
    }

    popup: T.Popup {
        y: control.height
        width: control.width

        height: Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin)
        topMargin: 6
        bottomMargin: 6

        contentItem: ListView {
            clip: true
            model: control.delegateModel
            implicitHeight: contentHeight
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0

            Rectangle {
                z: 10
                width: parent.width
                height: parent.height
                color: "transparent"
                border.width: bg.border.width
                border.color: strokeColor.normal
            }

            T.ScrollIndicator.vertical: ScrollIndicator {}
        }

        background: Rectangle {
            color: backgroundColor.normal
        }
    }
}
