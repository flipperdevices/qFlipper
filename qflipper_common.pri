NAME = qFlipper

unix:!macx {
    DEFINES += USB_BACKEND_LIBUSB
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0 zlib

} else:win32 {
    CONFIG -= debug_and_release
    DEFINES += USB_BACKEND_WIN32
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib

    !win32-g++: LIBS +=  -lSetupApi -lWinusb -lUser32
    else: LIBS += -lsetupapi -lwinusb

} else:macx {
    DEFINES += USB_BACKEND_LIBUSB
    PKG_CONFIG = /usr/local/bin/pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0 zlib

} else {
    error("Unsupported OS or compiler")
}

GIT_VERSION = $$system("git describe --tags --abbrev=0","lines", HAS_VERSION)
!equals(HAS_VERSION, 0) {
    GIT_VERSION = unknown
}

GIT_COMMIT = $$system("git rev-parse --short=8 HEAD","lines", HAS_COMMIT)
!equals(HAS_COMMIT, 0) {
    GIT_COMMIT = unknown
}

GIT_TIMESTAMP = $$system("git log -1 --pretty=format:%ct","lines", HAS_TIMESTAMP)
!equals(HAS_TIMESTAMP, 0) {
    GIT_TIMESTAMP = 0
}

DEFINES += APP_NAME=\\\"$$NAME\\\" APP_VERSION=\\\"$$GIT_VERSION\\\" APP_COMMIT=\\\"$$GIT_COMMIT\\\" APP_TIMESTAMP=$$GIT_TIMESTAMP
