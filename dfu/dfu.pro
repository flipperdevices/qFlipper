QT -= gui

TEMPLATE = lib
CONFIG += staticlib c++11

include(../qflipper_common.pri)

SOURCES += \
    device/stm32wb55/optionbytes.cpp \
    device/stm32wb55/stm32wb55.cpp \
    dfumemorylayout.cpp \
    dfusedevice.cpp \
    dfusefile.cpp \
    usbdeviceinfo.cpp

HEADERS += \
    device/stm32wb55.h \
    device/stm32wb55/optionbytes.h \
    device/stm32wb55/stm32wb55.h \
    dfumemorylayout.h \
    dfusedevice.h \
    dfusefile.h \
    macros.h \
    usbdevice.h \
    usbdeviceinfo.h

contains(DEFINES, USB_BACKEND_LIBUSB) {
    SOURCES += libusb/usbdevice.cpp \
               libusb/usbdevicedetector.cpp

    HEADERS += libusb/usbdevice.h \
               libusb/usbdevicedetector.h

} else:contains(DEFINES, USB_BACKEND_WIN32) {
    SOURCES += win32/usbdevice.cpp \
               win32/usbdevicedetector.cpp

    HEADERS += win32/usbdevice.h \
               win32/usbdevicedetector.h
} else {
    error("No USB backend configured")
}
