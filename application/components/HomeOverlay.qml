import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0

Item {
    id: overlay

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 150
        }
    }

    UpdateButton {
        id: updateButton

        x: 450
        y: 250
    }
}
