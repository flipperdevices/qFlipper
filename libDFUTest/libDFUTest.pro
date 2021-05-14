QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += \
        main.cpp

unix: LIBS += -L$$OUT_PWD/../libDFU/ -lusb-1.0 -llibDFU

INCLUDEPATH += $$PWD/../libDFU
DEPENDPATH += $$PWD/../libDFU

unix: PRE_TARGETDEPS += $$OUT_PWD/../libDFU/liblibDFU.a
