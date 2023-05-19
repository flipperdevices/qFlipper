pragma Singleton

import QtQuick 2.15

QtObject {
    // Workaround for QTBUG-107038
    readonly property bool fontRenderingFix: {
        if(Qt.platform.os === "linux") {
            if(qVersionMajor === 0x06) {
                if(qVersionMinor === 0x02) {
                    // Fix version: 6.2.7 (LTS)
                    return qVersionPatch < 0x7;
                } else if(qVersionMinor === 0x04) {
                    // Fix version: 6.4.1
                    return qVersionPatch < 0x1;
                } else {
                    // Versions beyond 6.5.0 should not be affected
                    return qVersionMinor < 5;
                }
            } else {
                // Qt 5 seems to be not affected
                return false;
            }

        } else {
            return false;
        }
    }
}
