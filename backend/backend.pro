QT -= gui
QT += serialport network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../qflipper_common.pri)

SOURCES += \
    abstractoperation.cpp \
    deviceregistry.cpp \
    firmwaredownloader.cpp \
    flipperupdates.cpp \
    flipperzero/deviceinfofetcher.cpp \
    flipperzero/factoryinfo.cpp \
    flipperzero/flipperzero.cpp \
    flipperzero/operations/assetsdownloadoperation.cpp \
    flipperzero/operations/firmwaredownloadoperation.cpp \
    flipperzero/operations/fixbootissuesoperation.cpp \
    flipperzero/operations/fixoptionbytesoperation.cpp \
    flipperzero/operations/flipperzerooperation.cpp \
    flipperzero/operations/wirelessstackdownloadoperation.cpp \
    flipperzero/recoverycontroller.cpp \
    flipperzero/remotecontroller.cpp \
    flipperzero/storage/statoperation.cpp \
    flipperzero/storage/storageoperation.cpp \
    flipperzero/storagecontroller.cpp \
    gzipuncompressor.cpp \
    qflipperbackend.cpp \
    remotefilefetcher.cpp \
    serialfinder.cpp \
    tararchive.cpp \
    updateregistry.cpp

HEADERS += \
    abstractoperation.h \
    deviceregistry.h \
    firmwaredownloader.h \
    flipperupdates.h \
    flipperzero/deviceinfo.h \
    flipperzero/deviceinfofetcher.h \
    flipperzero/factoryinfo.h \
    flipperzero/flipperzero.h \
    flipperzero/operations/assetsdownloadoperation.h \
    flipperzero/operations/firmwaredownloadoperation.h \
    flipperzero/operations/fixbootissuesoperation.h \
    flipperzero/operations/fixoptionbytesoperation.h \
    flipperzero/operations/flipperzerooperation.h \
    flipperzero/operations/wirelessstackdownloadoperation.h \
    flipperzero/recoverycontroller.h \
    flipperzero/remotecontroller.h \
    flipperzero/storage/statoperation.h \
    flipperzero/storage/storageoperation.h \
    flipperzero/storagecontroller.h \
    gzipuncompressor.h \
    qflipperbackend.h \
    remotefilefetcher.h \
    serialfinder.h \
    tararchive.h \
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

