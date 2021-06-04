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

unix: {
    SOURCES += \
        backends/libusbbackend.cpp

    HEADERS += \
        backends/libusbbackend.h
}
