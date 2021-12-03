QT -= gui
QT += serialport network

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

include(../qflipper_common.pri)

SOURCES += \
    abstractoperation.cpp \
    abstractoperationhelper.cpp \
    abstractoperationrunner.cpp \
    abstractserialoperation.cpp \
    deviceregistry.cpp \
    filenode.cpp \
    flipperupdates.cpp \
    flipperzero/assetmanifest.cpp \
    flipperzero/cli/deviceinfooperation.cpp \
    flipperzero/cli/dfuoperation.cpp \
    flipperzero/cli/factoryresetclioperation.cpp \
    flipperzero/cli/rebootoperation.cpp \
    flipperzero/cli/startrpcoperation.cpp \
    flipperzero/commandinterface.cpp \
    flipperzero/cli/skipmotdoperation.cpp \
    flipperzero/devicestate.cpp \
    flipperzero/factoryinfo.cpp \
    flipperzero/firmwareupdater.cpp \
    flipperzero/flipperzero.cpp \
    flipperzero/helper/deviceinfohelper.cpp \
    flipperzero/helper/firmwarehelper.cpp \
    flipperzero/helper/radiomanifesthelper.cpp \
    flipperzero/helper/scriptshelper.cpp \
    flipperzero/protobuf/application.pb.c \
    flipperzero/protobuf/flipper.pb.c \
    flipperzero/protobuf/gui.pb.c \
    flipperzero/protobuf/status.pb.c \
    flipperzero/protobuf/storage.pb.c \
    flipperzero/radiomanifest.cpp \
    flipperzero/recovery.cpp \
    flipperzero/recovery/abstractrecoveryoperation.cpp \
    flipperzero/recovery/correctoptionbytesoperation.cpp \
    flipperzero/recovery/exitrecoveryoperation.cpp \
    flipperzero/recovery/firmwaredownloadoperation.cpp \
    flipperzero/recovery/fixbootissuesoperation.cpp \
    flipperzero/recovery/radioupdateoperation.cpp \
    flipperzero/recovery/setbootmodeoperation.cpp \
    flipperzero/recovery/wirelessstackdownloadoperation.cpp \
    flipperzero/recoveryinterface.cpp \
    flipperzero/cli/listoperation.cpp \
    flipperzero/cli/mkdiroperation.cpp \
    flipperzero/cli/readoperation.cpp \
    flipperzero/cli/removeoperation.cpp \
    flipperzero/cli/statoperation.cpp \
    flipperzero/cli/writeoperation.cpp \
    flipperzero/screenstreamer.cpp \
    flipperzero/toplevel/abstracttopleveloperation.cpp \
    flipperzero/toplevel/factoryresetoperation.cpp \
    flipperzero/toplevel/firmwareinstalloperation.cpp \
    flipperzero/toplevel/fullrepairoperation.cpp \
    flipperzero/toplevel/fullupdateoperation.cpp \
    flipperzero/toplevel/settingsbackupoperation.cpp \
    flipperzero/toplevel/settingsrestoreoperation.cpp \
    flipperzero/toplevel/wirelessstackupdateoperation.cpp \
    flipperzero/utility/abstractutilityoperation.cpp \
    flipperzero/utility/assetsdownloadoperation.cpp \
    flipperzero/utility/factoryresetutiloperation.cpp \
    flipperzero/utility/getfiletreeoperation.cpp \
    flipperzero/utility/restartoperation.cpp \
    flipperzero/utility/startrecoveryoperation.cpp \
    flipperzero/utility/userbackupoperation.cpp \
    flipperzero/utility/userrestoreoperation.cpp \
    flipperzero/utilityinterface.cpp \
    gzipuncompressor.cpp \
    logger.cpp \
    preferences.cpp \
    qflipperbackend.cpp \
    remotefilefetcher.cpp \
    serialfinder.cpp \
    simpleserialoperation.cpp \
    tararchive.cpp \
    tarziparchive.cpp \
    tempdirectories.cpp \
    updateregistry.cpp

HEADERS += \
    abstractoperation.h \
    abstractoperationhelper.h \
    abstractoperationrunner.h \
    abstractserialoperation.h \
    deviceregistry.h \
    failable.h \
    fileinfo.h \
    filenode.h \
    flipperupdates.h \
    flipperzero/assetmanifest.h \
    flipperzero/cli/deviceinfooperation.h \
    flipperzero/cli/dfuoperation.h \
    flipperzero/cli/factoryresetclioperation.h \
    flipperzero/cli/rebootoperation.h \
    flipperzero/cli/startrpcoperation.h \
    flipperzero/commandinterface.h \
    flipperzero/cli/skipmotdoperation.h \
    flipperzero/deviceinfo.h \
    flipperzero/devicestate.h \
    flipperzero/factoryinfo.h \
    flipperzero/firmwareupdater.h \
    flipperzero/flipperzero.h \
    flipperzero/helper/deviceinfohelper.h \
    flipperzero/helper/firmwarehelper.h \
    flipperzero/helper/radiomanifesthelper.h \
    flipperzero/helper/scriptshelper.h \
    flipperzero/protobuf/application.pb.h \
    flipperzero/protobuf/flipper.pb.h \
    flipperzero/protobuf/gui.pb.h \
    flipperzero/protobuf/status.pb.h \
    flipperzero/protobuf/storage.pb.h \
    flipperzero/radiomanifest.h \
    flipperzero/recovery.h \
    flipperzero/recovery/abstractrecoveryoperation.h \
    flipperzero/recovery/correctoptionbytesoperation.h \
    flipperzero/recovery/exitrecoveryoperation.h \
    flipperzero/recovery/firmwaredownloadoperation.h \
    flipperzero/recovery/fixbootissuesoperation.h \
    flipperzero/recovery/radioupdateoperation.h \
    flipperzero/recovery/setbootmodeoperation.h \
    flipperzero/recovery/wirelessstackdownloadoperation.h \
    flipperzero/recoveryinterface.h \
    flipperzero/cli/listoperation.h \
    flipperzero/cli/mkdiroperation.h \
    flipperzero/cli/readoperation.h \
    flipperzero/cli/removeoperation.h \
    flipperzero/cli/statoperation.h \
    flipperzero/cli/writeoperation.h \
    flipperzero/screenstreamer.h \
    flipperzero/toplevel/abstracttopleveloperation.h \
    flipperzero/toplevel/factoryresetoperation.h \
    flipperzero/toplevel/firmwareinstalloperation.h \
    flipperzero/toplevel/fullrepairoperation.h \
    flipperzero/toplevel/fullupdateoperation.h \
    flipperzero/toplevel/settingsbackupoperation.h \
    flipperzero/toplevel/settingsrestoreoperation.h \
    flipperzero/toplevel/wirelessstackupdateoperation.h \
    flipperzero/utility/abstractutilityoperation.h \
    flipperzero/utility/assetsdownloadoperation.h \
    flipperzero/utility/factoryresetutiloperation.h \
    flipperzero/utility/getfiletreeoperation.h \
    flipperzero/utility/restartoperation.h \
    flipperzero/utility/startrecoveryoperation.h \
    flipperzero/utility/userbackupoperation.h \
    flipperzero/utility/userrestoreoperation.h \
    flipperzero/utilityinterface.h \
    gzipuncompressor.h \
    logger.h \
    preferences.h \
    qflipperbackend.h \
    remotefilefetcher.h \
    serialfinder.h \
    signalingfailable.h \
    simpleserialoperation.h \
    tararchive.h \
    tarziparchive.h \
    tempdirectories.h \
    updateregistry.h

unix|win32 {
    LIBS += -L$$OUT_PWD/../dfu/ -ldfu \
            -L$$OUT_PWD/../3rdparty/ -l3rdparty
}

win32:!win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/dfu.lib \
                      $$OUT_PWD/../3rdparty/3rdparty.lib

} else:unix|win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/libdfu.a \
                      $$OUT_PWD/../3rdparty/lib3rdparty.a
}

INCLUDEPATH += $$PWD/../dfu \
               $$PWD/../3rdparty \
               $$PWD/../3rdparty/nanopb

DEPENDPATH += $$PWD/../dfu \
              $$PWD/../3rdparty

