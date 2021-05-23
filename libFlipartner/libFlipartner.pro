QT -= gui
QT += serialport

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../Flipartner_common.pri)

SOURCES += \
    firmwareupdater.cpp \
    flipartnerbackend.cpp \
    flipperdetector.cpp \
    flipperlistmodel.cpp

HEADERS += \
    firmwareupdater.h \
    flipartnerbackend.h \
    flipperdetector.h \
    flipperinfo.h \
    flipperlistmodel.h

unix: {
    LIBS += -L$$OUT_PWD/../libDFU/ -llibDFU
    PRE_TARGETDEPS += $$OUT_PWD/../libDFU/liblibDFU.a
}

INCLUDEPATH += $$PWD/../libDFU
DEPENDPATH += $$PWD/../libDFU

