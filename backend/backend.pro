QT -= gui
QT += serialport network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../qflipper_common.pri)

SOURCES += \
    abstractfirmwareoperation.cpp \
    deviceregistry.cpp \
    firmwaredownloader.cpp \
    flipperupdates.cpp \
    flipperzero/deviceinfofetcher.cpp \
    flipperzero/factoryinfo.cpp \
    flipperzero/firmwareoperations.cpp \
    flipperzero/flipperzero.cpp \
    flipperzero/operations/firmwaredownloadoperation.cpp \
    flipperzero/operations/fixbootissuesoperation.cpp \
    flipperzero/operations/fixoptionbytesoperation.cpp \
    flipperzero/remotecontroller.cpp \
    qflipperbackend.cpp \
    remotefilefetcher.cpp \
    serialfinder.cpp \
    updateregistry.cpp

HEADERS += \
    abstractfirmwareoperation.h \
    deviceregistry.h \
    firmwaredownloader.h \
    flipperupdates.h \
    flipperzero/deviceinfo.h \
    flipperzero/deviceinfofetcher.h \
    flipperzero/factoryinfo.h \
    flipperzero/firmwareoperations.h \
    flipperzero/flipperzero.h \
    flipperzero/operations/firmwaredownloadoperation.h \
    flipperzero/operations/fixbootissuesoperation.h \
    flipperzero/operations/fixoptionbytesoperation.h \
    flipperzero/remotecontroller.h \
    qflipperbackend.h \
    remotefilefetcher.h \
    serialfinder.h \
    updateregistry.h

unix|win32 {
    LIBS += -L$$OUT_PWD/../dfu/ -ldfu
}

win32:!win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/dfu.lib
} else:unix|win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/libdfu.a
}

INCLUDEPATH += $$PWD/../dfu
DEPENDPATH += $$PWD/../dfu

