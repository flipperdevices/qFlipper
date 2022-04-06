QT -= gui
QT += serialport network

include(../qflipper_common.pri)

TARGET = $${NAME}-cli

DESTDIR = $$OUT_PWD/..
CONFIG += c++11 console
CONFIG -= app_bundle

unix|win32 {
    LIBS += \
        -L$$OUT_PWD/../backend/ -lbackend \
        -L$$OUT_PWD/../3rdparty/ -l3rdparty \
        -L$$OUT_PWD/../dfu/ -ldfu
}

win32:!win32-g++ {
    PRE_TARGETDEPS += \
        $$OUT_PWD/../backend/backend.lib \
        $$OUT_PWD/../3rdparty/3rdparty.lib \
        $$OUT_PWD/../dfu/dfu.lib

} else:unix|win32-g++ {
    PRE_TARGETDEPS += \
        $$OUT_PWD/../backend/libbackend.a \
        $$OUT_PWD/../3rdparty/lib3rdparty.a \
        $$OUT_PWD/../dfu/libdfu.a
}

win32 {
    equals(HAS_VERSION, 0) {
        RC_SUFFIX = -rc

        contains(GIT_VERSION, .*$${RC_SUFFIX}.*) {
            # Remove -rc suffix as it isn't allowed in Windows manifest
            TOKENS = $$split(GIT_VERSION, -)
            VERSION = $$first(TOKENS)
        } else {
            VERSION = $$GIT_VERSION
        }

    } else: VERSION = 0.0.0
}

macx: ICON = $$PWD/../application/assets/icons/$${NAME}.icns
else:win32: RC_ICONS = $$PWD/../application/assets/icons/$${NAME}-cli.ico

INCLUDEPATH += \
    $$PWD/../dfu \
    $$PWD/../backend

DEPENDPATH += \
    $$PWD/../dfu \
    $$PWD/../backend \
    $$PWD/../3rdparty \

SOURCES += \
        main.cpp \
        cli.cpp

HEADERS += \
    cli.h

unix:!macx {
    target.path = $$PREFIX/bin
} else:macx {
    target.path = $$DESTDIR/$${NAME}.app/Contents/MacOS
} else:win32 {
    target.path = $$DESTDIR/$$NAME
}

INSTALLS += target
