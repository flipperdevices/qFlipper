QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += \
        main.cpp

include(../Flipartner_common.pri)

unix: {
    LIBS += -L$$OUT_PWD/../libDFU/ -llibDFU
    PRE_TARGETDEPS += $$OUT_PWD/../libDFU/liblibDFU.a
}

INCLUDEPATH += $$PWD/../libDFU
DEPENDPATH += $$PWD/../libDFU

