unix:!macx {
    DEFINES += USB_BACKEND_LIBUSB
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0

} else:win32 {
    CONFIG -= debug_and_release

    contains(DEFINES, USB_BACKEND_LIBUSB) {
        INCLUDEPATH += $$LIBUSB_DIR/include

        !win32-g++: LIBS += -L$$LIBUSB_DIR/lib -llibusb-1.0
        else: LIBS += -L$$LIBUSB_DIR/lib -lusb-1.0

    } else {
        error("No USB backend specified. Current options are: USB_BACKEND_LIBUSB")
    }

} else:macx {
    DEFINES += USB_BACKEND_LIBUSB
    PKG_CONFIG = /usr/local/bin/pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0

} else {
    error("Unsupported OS or compiler")
}

