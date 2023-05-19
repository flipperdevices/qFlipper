QT -= gui

include(../../qflipper_common.pri)

win32: TARGET = flipperproto
else: TARGET = flipperproto0

DESTDIR = $$OUT_PWD/..

TEMPLATE = lib
CONFIG += plugin c++11

VERSION = 0.0.0

INCLUDEPATH += $$PWD/../protobufinterface \
    $$PWD/../../3rdparty/nanopb

HEADERS += \
    guirequest.h \
    guiresponse.h \
    mainrequest.h \
    mainresponse.h \
    messages/application.pb.h \
    messages/flipper.pb.h \
    messages/gui.pb.h \
    messages/property.pb.h \
    messages/status.pb.h \
    messages/storage.pb.h \
    messages/system.pb.h \
    messagewrapper.h \
    propertyrequest.h \
    propertyresponse.h \
    protobufplugin.h \
    regiondata.h \
    statusrequest.h \
    statusresponse.h \
    storagerequest.h \
    storageresponse.h \
    systemrequest.h \
    systemresponse.h

SOURCES += \
    guirequest.cpp \
    guiresponse.cpp \
    mainrequest.cpp \
    mainresponse.cpp \
    messages/application.pb.c \
    messages/flipper.pb.c \
    messages/gpio.pb.c \
    messages/gui.pb.c \
    messages/property.pb.c \
    messages/status.pb.c \
    messages/storage.pb.c \
    messages/system.pb.c \
    messagewrapper.cpp \
    propertyrequest.cpp \
    propertyresponse.cpp \
    protobufplugin.cpp \
    regiondata.cpp \
    statusrequest.cpp \
    statusresponse.cpp \
    storagerequest.cpp \
    storageresponse.cpp \
    systemrequest.cpp \
    systemresponse.cpp

unix|win32 {
    LIBS += -L$$OUT_PWD/../../3rdparty/ -l3rdparty
}

DEFINES += PB_ENABLE_MALLOC

!contains(CONFIG, static) {
    unix:!macx {
        target.path = $$PREFIX/lib/$$NAME/plugins
    } else:macx {
        target.path = $$DESTDIR/../$${NAME}.app/Contents/PlugIns
    } else:win32 {
        target.path = $$DESTDIR/../$$NAME/plugins
    }

    INSTALLS += target
}

