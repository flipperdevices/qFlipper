pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property var icons: {
        "ir"  : "qrc:/assets/gfx/symbolic/mimetypes/infrared.svg",
        "nfc" : "qrc:/assets/gfx/symbolic/mimetypes/nfc.svg",
        "u2f" : "qrc:/assets/gfx/symbolic/mimetypes/u2f.svg",
        "sub" : "qrc:/assets/gfx/symbolic/mimetypes/subghz.svg",
        "rfid" : "qrc:/assets/gfx/symbolic/mimetypes/rfid.svg",
        "ibtn" : "qrc:/assets/gfx/symbolic/mimetypes/ibutton.svg",
        "default" : "qrc:/assets/gfx/symbolic/mimetypes/file.svg"
    }
}
