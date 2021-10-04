QT -= gui
QT += serialport network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../qflipper_common.pri)

SOURCES += \
    abstractoperation.cpp \
    abstractoperationrunner.cpp \
    abstractserialoperation.cpp \
    deviceregistry.cpp \
    filenode.cpp \
    firmwaredownloader.cpp \
    flipperupdates.cpp \
    flipperzero/assetmanifest.cpp \
    flipperzero/cli/dfuoperation.cpp \
    flipperzero/commandinterface.cpp \
    flipperzero/cli/skipmotdoperation.cpp \
    flipperzero/deviceinfofetcher.cpp \
    flipperzero/devicestate.cpp \
    flipperzero/factoryinfo.cpp \
    flipperzero/firmwareupdater.cpp \
    flipperzero/flipperzero.cpp \
    flipperzero/flipperzerooperation.cpp \
    flipperzero/recovery.cpp \
    flipperzero/recovery/abstractrecoveryoperation.cpp \
    flipperzero/recovery/firmwaredownloadoperation.cpp \
    flipperzero/recovery/fixbootissuesoperation.cpp \
    flipperzero/recovery/fixoptionbytesoperation.cpp \
    flipperzero/recovery/leaveoperation.cpp \
    flipperzero/recovery/wirelessstackdownloadoperation.cpp \
    flipperzero/recoveryinterface.cpp \
    flipperzero/screenstreaminterface.cpp \
    flipperzero/cli/listoperation.cpp \
    flipperzero/cli/mkdiroperation.cpp \
    flipperzero/cli/readoperation.cpp \
    flipperzero/cli/removeoperation.cpp \
    flipperzero/cli/statoperation.cpp \
    flipperzero/cli/writeoperation.cpp \
    flipperzero/utility/assetsdownloadoperation.cpp \
    flipperzero/utility/getfiletreeoperation.cpp \
    flipperzero/utility/userbackupoperation.cpp \
    flipperzero/utility/userrestoreoperation.cpp \
    gzipuncompressor.cpp \
    qflipperbackend.cpp \
    remotefilefetcher.cpp \
    serialfinder.cpp \
    simpleserialoperation.cpp \
    tararchive.cpp \
    updateregistry.cpp

HEADERS += \
    abstractoperation.h \
    abstractoperationrunner.h \
    abstractserialoperation.h \
    deviceregistry.h \
    failable.h \
    fileinfo.h \
    filenode.h \
    firmwaredownloader.h \
    flipperupdates.h \
    flipperzero/assetmanifest.h \
    flipperzero/cli/dfuoperation.h \
    flipperzero/commandinterface.h \
    flipperzero/cli/skipmotdoperation.h \
    flipperzero/deviceinfo.h \
    flipperzero/deviceinfofetcher.h \
    flipperzero/devicestate.h \
    flipperzero/factoryinfo.h \
    flipperzero/firmwareupdater.h \
    flipperzero/flipperzero.h \
    flipperzero/flipperzerooperation.h \
    flipperzero/recovery.h \
    flipperzero/recovery/abstractrecoveryoperation.h \
    flipperzero/recovery/firmwaredownloadoperation.h \
    flipperzero/recovery/fixbootissuesoperation.h \
    flipperzero/recovery/fixoptionbytesoperation.h \
    flipperzero/recovery/leaveoperation.h \
    flipperzero/recovery/wirelessstackdownloadoperation.h \
    flipperzero/recoveryinterface.h \
    flipperzero/screenstreaminterface.h \
    flipperzero/cli/listoperation.h \
    flipperzero/cli/mkdiroperation.h \
    flipperzero/cli/readoperation.h \
    flipperzero/cli/removeoperation.h \
    flipperzero/cli/statoperation.h \
    flipperzero/cli/writeoperation.h \
    flipperzero/utility/assetsdownloadoperation.h \
    flipperzero/utility/getfiletreeoperation.h \
    flipperzero/utility/userbackupoperation.h \
    flipperzero/utility/userrestoreoperation.h \
    gzipuncompressor.h \
    qflipperbackend.h \
    remotefilefetcher.h \
    serialfinder.h \
    signalingfailable.h \
    simpleserialoperation.h \
    tararchive.h \
    updateregistry.h

unix|win32 {
    LIBS += -L$$OUT_PWD/../dfu/ -ldfu
}

win32:!win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/dfu.lib
} else:unix|win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/libdfu.a
}

INCLUDEPATH += $$PWD/../dfu
DEPENDPATH += $$PWD/../dfu

