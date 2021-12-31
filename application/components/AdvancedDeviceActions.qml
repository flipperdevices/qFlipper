import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Theme 1.0
import QFlipper 1.0

ColumnLayout {
    id: control
    spacing: 10

    property alias installRadioAction: installRadioAction
    property alias installFusAction: installFusAction

    TransparentLabel {
        color: Theme.color.lightorange2
        text: qsTr("Proceed with extreme caution")
    }

    SmallButtonRed {
        action: installRadioAction
        Layout.fillWidth: true

        icon.source: "qrc:/assets/gfx/symbolic/restore-symbolic.svg"
        icon.width: 18
        icon.height: 20

        ToolTip {
            visible: parent.hovered
            text: qsTr("Install a custom wireless stack. WARNING! This may result in a non-functional Flipper!")
            implicitWidth: 250
        }
    }

    SmallButtonRed {
        action: installFusAction
        Layout.fillWidth: true

        icon.source: "qrc:/assets/gfx/symbolic/restore-symbolic.svg"
        icon.width: 18
        icon.height: 20

        ToolTip {
            visible: parent.hovered
            text: qsTr("Install a custom FUS firmware. !!MEGA WARNING!! It WILL result in encryption keys loss!")
            implicitWidth: 250
        }
    }

    TransparentLabel {
        color: Theme.color.lightorange2
        text: qsTr("Proceed with extreme caution")
    }

    Action {
        id: installRadioAction
        text: qsTr("Install wireless fw")
    }

    Action {
        id: installFusAction
        text: qsTr("Install FUS firmware")
        enabled: Backend.deviceState && Backend.deviceState.isRecoveryMode
    }
}
