QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

SOURCES += \
    abstractusbdevicebackend.cpp \
    dfumemorylayout.cpp \
    dfusedevice.cpp \
    dfusefile.cpp \
    libusbusbdevicebackend.cpp \
    usbdevice.cpp \
    usbdeviceinfo.cpp

HEADERS += \
    abstractusbdevicebackend.h \
    descriptors.h \
    dfumemorylayout.h \
    dfusedevice.h \
    dfusefile.h \
    libusbusbdevicebackend.h \
    macros.h \
    usbdevice.h \
    usbdeviceinfo.h \
    usbdevicelocation.h

INCLUDEPATH += /usr/include/libusb-1.0
