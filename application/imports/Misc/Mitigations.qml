pragma Singleton

import QtQuick 2.15

QtObject {
    // Workaround for QTBUG-107038
    readonly property bool fontRenderingFix: Qt.platform.os === "linux" && qVersion >= 0x060000
}
