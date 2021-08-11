QT += quick serialport widgets quickcontrols2 svg

TARGET = qFlipper

GIT_VERSION = $$system("git describe --tags --abbrev=0","lines", HAS_VERSION)

equals(HAS_VERSION, 0) {
    RC_SUFFIX = -rc

    contains(GIT_VERSION, .*$$RC_SUFFIX) {
        # Remove -rc suffix as it isn't allowed in Windows manifest
        VERSION = $$str_member($$GIT_VERSION, 0, $$num_add($$str_size($$GIT_VERSION), -$$num_add($$str_size($$RC_SUFFIX), 1)))
    } else {
        VERSION = $$GIT_VERSION
    }

} else {
    VERSION = 0.0.0
    GIT_VERSION = unknown
}

DEFINES += APP_VERSION=\\\"$$GIT_VERSION\\\"
DESTDIR = ..

CONFIG += c++11

SOURCES += \
        main.cpp \
        screencanvas.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    translations/en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

unix|win32 {
    LIBS += \
        -L$$OUT_PWD/../backend/ -lbackend \
        -L$$OUT_PWD/../dfu/ -ldfu
}

win32:!win32-g++ {
    PRE_TARGETDEPS += \
        $$OUT_PWD/../backend/backend.lib \
        $$OUT_PWD/../dfu/dfu.lib

} else:unix|win32-g++ {
    PRE_TARGETDEPS += \
        $$OUT_PWD/../backend/libbackend.a \
        $$OUT_PWD/../dfu/libdfu.a
}

include(../qflipper_common.pri)

macx: ICON = assets/icons/qFlipper.icns
else:win32: RC_ICONS = assets/icons/qFlipper.ico

INCLUDEPATH += \
    $$PWD/../dfu \
    $$PWD/../backend

DEPENDPATH += \
    $$PWD/../dfu \
    $$PWD/../backend

HEADERS += \
    screencanvas.h

DISTFILES +=
