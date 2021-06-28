QT -= gui
QT += serialport network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../flipartner_common.pri)

SOURCES += \
    deviceregistry.cpp \
    firmwaredownloader.cpp \
    flipartnerbackend.cpp \
    flipperupdates.cpp \
    flipperzero.cpp \
    flipperzeroremote.cpp \
    remotefilefetcher.cpp \
    updateregistry.cpp

HEADERS += \
    deviceregistry.h \
    firmwaredownloader.h \
    flipartnerbackend.h \
    flipperupdates.h \
    flipperzero.h \
    flipperzeroremote.h \
    remotefilefetcher.h \
    serialhelper.h \
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

