unix:!macx {
    DEFINES += USB_BACKEND_LIBUSB
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0

} else:win32-g++ {
    CONFIG -= debug_and_release
    DEFINES += USB_BACKEND_WIN32
    LIBS += -lsetupapi -lwinusb

} else {
    error("Unsupported OS or compiler")
}
