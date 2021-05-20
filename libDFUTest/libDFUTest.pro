QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += \
        main.cpp

include(../Flipartner_common.pri)

unix: LIBS += -L$$OUT_PWD/../libDFU/ -llibDFU

INCLUDEPATH += $$PWD/../libDFU
DEPENDPATH += $$PWD/../libDFU

unix: PRE_TARGETDEPS += $$OUT_PWD/../libDFU/liblibDFU.a
