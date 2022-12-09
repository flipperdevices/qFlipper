QT -= gui

TEMPLATE = lib
CONFIG += staticlib c++11

requires(false)

HEADERS += \
    bandinfo.h \
    guiresponseinterface.h \
    mainresponseinterface.h \
    propertyresponseinterface.h \
    protobufplugininterface.h \
    statusresponseinterface.h \
    storageresponseinterface.h \
    systemresponseinterface.h
