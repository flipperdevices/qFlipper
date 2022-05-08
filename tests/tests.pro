QT += testlib
QT -= gui

include(../qflipper_common.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testversion.cpp

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

INCLUDEPATH += \
    $$PWD/../dfu \
    $$PWD/../backend

DEPENDPATH += \
    $$PWD/../dfu \
    $$PWD/../backend
