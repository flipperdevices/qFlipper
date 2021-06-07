QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

SOURCES += \
    dfumemorylayout.cpp \
    dfusedevice.cpp \
    dfusefile.cpp \
    usbdevice.cpp

HEADERS += \
    descriptors.h \
    dfumemorylayout.h \
    dfusedevice.h \
    dfusefile.h \
    macros.h \
    usbbackend.h \
    usbdevice.h \
    usbdeviceparams.h

include(../Flipartner_common.pri)

contains(DEFINES, USB_BACKEND_LIBUSB) {
    SOURCES += backends/libusbbackend.cpp
    HEADERS += backends/libusbbackend.h
} else {
    error("No USB backend configured")
}
