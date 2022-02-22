QT += quick serialport widgets quickcontrols2 svg

include(../qflipper_common.pri)

TARGET = $$NAME
DESTDIR = ..

CONFIG += c++11

SOURCES += \
        application.cpp \
        applicationupdater.cpp \
        applicationupdateregistry.cpp \
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
QML_IMPORT_PATH = $$PWD/imports

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

macx: ICON = assets/icons/$${NAME}.icns
else:win32: RC_ICONS = assets/icons/$${NAME}.ico

INCLUDEPATH += \
    $$PWD/../dfu \
    $$PWD/../backend

DEPENDPATH += \
    $$PWD/../dfu \
    $$PWD/../backend

HEADERS += \
    application.h \
    applicationupdater.h \
    applicationupdateregistry.h \
    screencanvas.h

DISTFILES +=

unix:!macx {
    target.path = $$PREFIX/bin

    desktopfiles.files = $$PWD/../installer-assets/appimage/$${TARGET}.desktop
    desktopfiles.path = $$PREFIX/share/applications

    iconfiles.files = $$PWD/assets/icons/$${TARGET}.png
    iconfiles.path = $$PREFIX/share/icons/hicolor/512x512/apps

    INSTALLS += target desktopfiles iconfiles
}
