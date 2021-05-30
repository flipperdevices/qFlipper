QT -= gui
QT += serialport

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../Flipartner_common.pri)

SOURCES += \
    firmwaredownloadtask.cpp \
    firmwareupdater.cpp \
    flipartnerbackend.cpp \
    flipperdetector.cpp \
    flipperinfotask.cpp \
    flipperlistmodel.cpp \
    flipperupdates.cpp \
    flipperzero.cpp \
    informationfetcher.cpp \
    registry.cpp \
    serialhelper.cpp \
    updateslistmodel.cpp

HEADERS += \
    firmwaredownloadtask.h \
    firmwareupdater.h \
    flipartnerbackend.h \
    flipperdetector.h \
    flipperinfo.h \
    flipperinfotask.h \
    flipperlistmodel.h \
    flipperupdates.h \
    flipperzero.h \
    informationfetcher.h \
    registry.h \
    serialhelper.h \
    updateslistmodel.h

unix: {
    LIBS += -L$$OUT_PWD/../libDFU/ -llibDFU
    PRE_TARGETDEPS += $$OUT_PWD/../libDFU/liblibDFU.a
}

INCLUDEPATH += $$PWD/../libDFU
DEPENDPATH += $$PWD/../libDFU

