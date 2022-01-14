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
    abstractprotobufoperation.cpp \
    abstractserialoperation.cpp \
    applicationbackend.cpp \
    deviceregistry.cpp \
    filenode.cpp \
    firmwareupdateregistry.cpp \
    flipperupdates.cpp \
    flipperzero/assetmanifest.cpp \
    flipperzero/rpc/guistartstreamoperation.cpp \
    flipperzero/rpc/guistopstreamoperation.cpp \
    flipperzero/rpc/startrpcoperation.cpp \
    flipperzero/rpc/stoprpcoperation.cpp \
    flipperzero/rpc/storageinfooperation.cpp \
    flipperzero/rpc/storagelistoperation.cpp \
    flipperzero/rpc/storagemkdiroperation.cpp \
    flipperzero/rpc/storagereadoperation.cpp \
    flipperzero/rpc/storageremoveoperation.cpp \
    flipperzero/rpc/storagestatoperation.cpp \
    flipperzero/rpc/storagewriteoperation.cpp \
    flipperzero/rpc/systemdeviceinfooperation.cpp \
    flipperzero/rpc/systemfactoryresetoperation.cpp \
    flipperzero/rpc/systemgetdatetimeoperation.cpp \
    flipperzero/rpc/systemrebootoperation.cpp \
    flipperzero/rpc/systemsetdatetimeoperation.cpp \
    flipperzero/commandinterface.cpp \
    flipperzero/rpc/skipmotdoperation.cpp \
    flipperzero/devicestate.cpp \
    flipperzero/factoryinfo.cpp \
    flipperzero/flipperzero.cpp \
    flipperzero/helper/deviceinfohelper.cpp \
    flipperzero/helper/firmwarehelper.cpp \
    flipperzero/helper/radiomanifesthelper.cpp \
    flipperzero/helper/scriptshelper.cpp \
    flipperzero/helper/serialinithelper.cpp \
    flipperzero/helper/toplevelhelper.cpp \
    flipperzero/protobuf/guiprotobufmessage.cpp \
    flipperzero/protobuf/mainprotobufmessage.cpp \
    flipperzero/protobuf/messages/application.pb.c \
    flipperzero/protobuf/messages/flipper.pb.c \
    flipperzero/protobuf/messages/gui.pb.c \
    flipperzero/protobuf/messages/status.pb.c \
    flipperzero/protobuf/messages/storage.pb.c \
    flipperzero/protobuf/messages/system.pb.c \
    flipperzero/protobuf/storageprotobufmessage.cpp \
    flipperzero/protobuf/systemprotobufmessage.cpp \
    flipperzero/radiomanifest.cpp \
    flipperzero/recovery.cpp \
    flipperzero/recovery/abstractrecoveryoperation.cpp \
    flipperzero/recovery/correctoptionbytesoperation.cpp \
    flipperzero/recovery/exitrecoveryoperation.cpp \
    flipperzero/recovery/firmwaredownloadoperation.cpp \
    flipperzero/recovery/fixbootissuesoperation.cpp \
    flipperzero/recovery/setbootmodeoperation.cpp \
    flipperzero/recovery/wirelessstackdownloadoperation.cpp \
    flipperzero/recoveryinterface.cpp \
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
    abstractprotobufmessage.h \
    abstractprotobufoperation.h \
    abstractserialoperation.h \
    applicationbackend.h \
    deviceregistry.h \
    failable.h \
    fileinfo.h \
    filenode.h \
    firmwareupdateregistry.h \
    flipperupdates.h \
    flipperzero/assetmanifest.h \
    flipperzero/rpc/guistartstreamoperation.h \
    flipperzero/rpc/guistopstreamoperation.h \
    flipperzero/rpc/startrpcoperation.h \
    flipperzero/rpc/stoprpcoperation.h \
    flipperzero/rpc/storageinfooperation.h \
    flipperzero/rpc/storagelistoperation.h \
    flipperzero/rpc/storagemkdiroperation.h \
    flipperzero/rpc/storagereadoperation.h \
    flipperzero/rpc/storageremoveoperation.h \
    flipperzero/rpc/storagestatoperation.h \
    flipperzero/rpc/storagewriteoperation.h \
    flipperzero/rpc/systemdeviceinfooperation.h \
    flipperzero/rpc/systemfactoryresetoperation.h \
    flipperzero/rpc/systemgetdatetimeoperation.h \
    flipperzero/rpc/systemrebootoperation.h \
    flipperzero/rpc/systemsetdatetimeoperation.h \
    flipperzero/commandinterface.h \
    flipperzero/rpc/skipmotdoperation.h \
    flipperzero/deviceinfo.h \
    flipperzero/devicestate.h \
    flipperzero/factoryinfo.h \
    flipperzero/flipperzero.h \
    flipperzero/helper/deviceinfohelper.h \
    flipperzero/helper/firmwarehelper.h \
    flipperzero/helper/radiomanifesthelper.h \
    flipperzero/helper/scriptshelper.h \
    flipperzero/helper/serialinithelper.h \
    flipperzero/helper/toplevelhelper.h \
    flipperzero/protobuf/guiprotobufmessage.h \
    flipperzero/protobuf/mainprotobufmessage.h \
    flipperzero/protobuf/messages/application.pb.h \
    flipperzero/protobuf/messages/flipper.pb.h \
    flipperzero/protobuf/messages/gui.pb.h \
    flipperzero/protobuf/messages/status.pb.h \
    flipperzero/protobuf/messages/storage.pb.h \
    flipperzero/protobuf/messages/system.pb.h \
    flipperzero/protobuf/storageprotobufmessage.h \
    flipperzero/protobuf/systemprotobufmessage.h \
    flipperzero/radiomanifest.h \
    flipperzero/recovery.h \
    flipperzero/recovery/abstractrecoveryoperation.h \
    flipperzero/recovery/correctoptionbytesoperation.h \
    flipperzero/recovery/exitrecoveryoperation.h \
    flipperzero/recovery/firmwaredownloadoperation.h \
    flipperzero/recovery/fixbootissuesoperation.h \
    flipperzero/recovery/setbootmodeoperation.h \
    flipperzero/recovery/wirelessstackdownloadoperation.h \
    flipperzero/recoveryinterface.h \
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
