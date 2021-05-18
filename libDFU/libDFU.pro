QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

SOURCES += \
    abstractusbdevicebackend.cpp \
    dfudevice.cpp \
    dfumemorylayout.cpp \
    libusbusbdevicebackend.cpp \
    usbdevice.cpp \
    usbdeviceinfo.cpp

HEADERS += \
    abstractusbdevicebackend.h \
    descriptors.h \
    dfudevice.h \
    dfumemorylayout.h \
    libusbusbdevicebackend.h \
    macros.h \
    usbdevice.h \
    usbdeviceinfo.h \
    usbdevicelocation.h

INCLUDEPATH += /usr/include/libusb-1.0
