QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

SOURCES += \
    abstractusbdevicebackend.cpp \
    dfudevice.cpp \
    libusbusbdevicebackend.cpp \
    usbdevice.cpp \
    usbdeviceinfo.cpp

HEADERS += \
    abstractusbdevicebackend.h \
    descriptors.h \
    dfudevice.h \
    libusbusbdevicebackend.h \
    usbdevice.h \
    usbdeviceinfo.h \
    usbdevicelocation.h

INCLUDEPATH += /usr/include/libusb-1.0
