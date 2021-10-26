import QtQuick 2.15
import QtGraphicalEffects 1.15
import Theme 1.0

Rectangle {
    id: container

    radius: 9
    border.width: 3
    border.color: Theme.color.orange

    gradient: Gradient {
        GradientStop { position: 0; color: "#090400" }
        GradientStop { position: 1; color: "#210F00" }
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
