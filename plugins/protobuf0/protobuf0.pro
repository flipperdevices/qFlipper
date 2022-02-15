QT -= gui

TARGET = protobuf0
DESTDIR = ..

TEMPLATE = lib
CONFIG += plugin c++11

VERSION = 0.0.0

INCLUDEPATH += $$PWD/../protobufinterface \
    $$PWD/../../3rdparty/nanopb

HEADERS += \
    guirequest.h \
    mainrequest.h \
    mainresponse.h \
    messages/application.pb.h \
    messages/flipper.pb.h \
    messages/gui.pb.h \
    messages/status.pb.h \
    messages/storage.pb.h \
    messages/system.pb.h \
    messagewrapper.h \
    protobufplugin.h \
    statusrequest.h \
    statusresponse.h \
    storagerequest.h \
    storageresponse.h \
    systemrequest.h \
    systemresponse.h

SOURCES += \
    guirequest.cpp \
    mainrequest.cpp \
    mainresponse.cpp \
    messages/application.pb.c \
    messages/flipper.pb.c \
    messages/gui.pb.c \
    messages/status.pb.c \
    messages/storage.pb.c \
    messages/system.pb.c \
    messagewrapper.cpp \
    protobufplugin.cpp \
    statusrequest.cpp \
    statusresponse.cpp \
    storagerequest.cpp \
    storageresponse.cpp \
    systemrequest.cpp \
    systemresponse.cpp

unix|win32 {
    LIBS += -L$$OUT_PWD/../../3rdparty/ -l3rdparty
}

win32:!win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/3rdparty.lib

} else:unix|win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../../3rdparty/lib3rdparty.a
}

DEFINES += PB_ENABLE_MALLOC
