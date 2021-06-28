QT += quick serialport widgets quickcontrols2

TARGET = flipartner
DESTDIR = ..

CONFIG += c++11

SOURCES += \
        main.cpp \
        screencanvas.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    Application_en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

unix|win32 {
    LIBS += \
        -L$$OUT_PWD/../Backend/ -lBackend \
        -L$$OUT_PWD/../Dfu/ -lDfu
}

win32:!win32-g++ {
    PRE_TARGETDEPS += \
        $$OUT_PWD/../Backend/Backend.lib \
        $$OUT_PWD/../Dfu/Dfu.lib

} else:unix|win32-g++ {
    PRE_TARGETDEPS += \
        $$OUT_PWD/../Backend/libBackend.a \
        $$OUT_PWD/../Dfu/libDfu.a
}

include(../Flipartner_common.pri)

INCLUDEPATH += \
    $$PWD/../Dfu \
    $$PWD/../Backend

DEPENDPATH += \
    $$PWD/../Dfu \
    $$PWD/../Backend

HEADERS += \
    screencanvas.h
