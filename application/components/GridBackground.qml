import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15

import Theme 1.0

Rectangle {
    id: container

    radius: 9
    border.width: 3
    border.color: Theme.color.lightorange2

    gradient: Gradient {
        GradientStop { position: 0; color: "#090400" }
        GradientStop { position: 1; color: "#210F00" }
    }

    Item {
        anchors.fill: parent
        anchors.margins: container.border.width
        clip: true

        Rectangle {
            id: scanline
            width: parent.width
            height: 320
            gradient: Gradient {
                GradientStop {position: 0;     color: Color.transparent(Theme.color.lightorange2, 0)}
                GradientStop {position: 0.6;   color: Color.transparent(Theme.color.lightorange2, 0.01)}
                GradientStop {position: 0.99;  color: Color.transparent(Theme.color.lightorange2, 0.07)}
                GradientStop {position: 1;     color: Color.transparent(Theme.color.lightorange2, 0)}
            }
        }

        PropertyAnimation {
            target: scanline

            property: "y"
            duration: 4000

            from: -scanline.height
            to: container.height

            loops: Animation.Infinite
            easing.type: Easing.Linear
            running: true
        }
    }

    Canvas {
        anchors.fill: parent
        anchors.margins: container.border.width

        opacity: 0.15

        onPaint: {
            const numCells = 30;
            const cellSize = width / numCells;

            const ctx = getContext("2d");
            ctx.strokeStyle = "#aa5115";
            ctx.lineWidth = 2;

            for(let ypos = cellSize; ypos < height; ypos += cellSize) {
                const pos = Math.floor(ypos);
                ctx.moveTo(0, pos);
                ctx.lineTo(width, pos);
            }

            for(let xpos = cellSize; xpos < width; xpos += cellSize) {
                const pos = Math.floor(xpos);
                ctx.moveTo(pos, 0);
                ctx.lineTo(pos, height);
            }

            ctx.stroke();
        }
    }
}
