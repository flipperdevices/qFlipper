unix:!macx {
    DEFINES += USB_BACKEND_LIBUSB
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0

} else:win32 {
    CONFIG -= debug_and_release
    DEFINES += USB_BACKEND_WIN32
	
        !win32-g++: LIBS +=  -lSetupApi -lWinusb -lUser32
	else: LIBS += -lsetupapi -lwinusb

} else:macx {
    DEFINES += USB_BACKEND_LIBUSB
    PKG_CONFIG = /usr/local/bin/pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0

} else {
    error("Unsupported OS or compiler")
}

