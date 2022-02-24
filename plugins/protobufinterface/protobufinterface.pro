QT -= gui

TEMPLATE = lib
CONFIG += staticlib c++11

requires(false)

HEADERS += \
    guiresponseinterface.h \
    mainresponseinterface.h \
    protobufplugininterface.h \
    statusresponseinterface.h \
    storageresponseinterface.h \
    systemresponseinterface.h
