import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

RowLayout {
    id: control

    enum Style {
        Windows,
        MacOS,
        Linux
    }

    signal minimizeRequested
    signal closeRequested

    property bool closeEnabled: true
    property string controlPath

    readonly property int style: {
        // TODO: additional Linux style?
        if(Qt.platform.os === "osx") {
            return WindowControls.Style.MacOS
        } else {
            return WindowControls.Style.Windows
        }
    }

    readonly property string iconPath: {
        const p = controlPath + "/%1";

        switch(style) {
        case WindowControls.Style.MacOS:
           return p.arg("mac");

        case WindowControls.Style.Windows: default:
           return p.arg("windows");
        }
    }

    layoutDirection: {
        switch(style) {
        case WindowControls.Style.MacOS:
            return Qt.RightToLeft;

        case WindowControls.Style.Windows: default:
            return Qt.LeftToRight;
        }
    }

    spacing: {
        switch(style) {
        case WindowControls.Style.MacOS:
           return 8;

        case WindowControls.Style.Windows: default:
           return 12;
        }
    }

    DragHandler {
        target: null
        onActiveChanged: if(active) control.Window.window.startSystemMove();
    }

    Item {
        id: spacer
        Layout.fillWidth: true
    }

    ImageButton {
        id: minimizeButton

        iconPath: control.iconPath
        iconName: "minimize"

        onClicked: minimizeRequested()
    }

    ImageButton {
        id: closeButton
        enabled: control.closeEnabled

        iconPath: control.iconPath
        iconName: "close"

        onClicked: closeRequested()
    }
}
