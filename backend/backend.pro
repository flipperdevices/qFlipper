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
    applicationbackend.cpp \
    deviceregistry.cpp \
    failable.cpp \
    filenode.cpp \
    firmwareupdateregistry.cpp \
    flipperupdates.cpp \
    flipperzero/assetmanifest.cpp \
    flipperzero/filemanager.cpp \
    flipperzero/protobufsession.cpp \
    flipperzero/rpc/abstractprotobufoperation.cpp \
    flipperzero/rpc/guiscreenframeoperation.cpp \
    flipperzero/rpc/guisendinputoperation.cpp \
    flipperzero/rpc/guistartscreenstreamoperation.cpp \
    flipperzero/rpc/guistartvirtualdisplayoperation.cpp \
    flipperzero/rpc/guistopscreenstreamoperation.cpp \
    flipperzero/rpc/guistopvirtualdisplayoperation.cpp \
    flipperzero/rpc/startrpcoperation.cpp \
    flipperzero/rpc/stoprpcoperation.cpp \
    flipperzero/rpc/storageinfooperation.cpp \
    flipperzero/rpc/storagelistoperation.cpp \
    flipperzero/rpc/storagemkdiroperation.cpp \
    flipperzero/rpc/storagereadoperation.cpp \
    flipperzero/rpc/storageremoveoperation.cpp \
    flipperzero/rpc/storagerenameoperation.cpp \
    flipperzero/rpc/storagestatoperation.cpp \
    flipperzero/rpc/storagewriteoperation.cpp \
    flipperzero/rpc/systemdeviceinfooperation.cpp \
    flipperzero/rpc/systemfactoryresetoperation.cpp \
    flipperzero/rpc/systemgetdatetimeoperation.cpp \
    flipperzero/rpc/systemrebootoperation.cpp \
    flipperzero/rpc/systemsetdatetimeoperation.cpp \
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
    flipperzero/radiomanifest.cpp \
    flipperzero/recovery.cpp \
    flipperzero/recovery/abstractrecoveryoperation.cpp \
    flipperzero/recovery/correctoptionbytesoperation.cpp \
    flipperzero/recovery/exitrecoveryoperation.cpp \
    flipperzero/recovery/firmwaredownloadoperation.cpp \
    flipperzero/recovery/setbootmodeoperation.cpp \
    flipperzero/recovery/wirelessstackdownloadoperation.cpp \
    flipperzero/recoveryinterface.cpp \
    flipperzero/rpc/systemupdateoperation.cpp \
    flipperzero/screenstreamer.cpp \
    flipperzero/toplevel/abstracttopleveloperation.cpp \
    flipperzero/toplevel/factoryresetoperation.cpp \
    flipperzero/toplevel/firmwareinstalloperation.cpp \
    flipperzero/toplevel/fullrepairoperation.cpp \
    flipperzero/toplevel/fullupdateoperation.cpp \
    flipperzero/toplevel/internalupdateoperation.cpp \
    flipperzero/toplevel/settingsbackupoperation.cpp \
    flipperzero/toplevel/settingsrestoreoperation.cpp \
    flipperzero/toplevel/wirelessstackupdateoperation.cpp \
    flipperzero/utility/abstractutilityoperation.cpp \
    flipperzero/utility/assetsdownloadoperation.cpp \
    flipperzero/utility/directorydownloadoperation.cpp \
    flipperzero/utility/directoryuploadoperation.cpp \
    flipperzero/utility/factoryresetutiloperation.cpp \
    flipperzero/utility/getfiletreeoperation.cpp \
    flipperzero/utility/restartoperation.cpp \
    flipperzero/utility/startrecoveryoperation.cpp \
    flipperzero/utility/startupdateroperation.cpp \
    flipperzero/utility/updateprepareoperation.cpp \
    flipperzero/utility/userbackupoperation.cpp \
    flipperzero/utility/userrestoreoperation.cpp \
    flipperzero/utilityinterface.cpp \
    flipperzero/virtualdisplay.cpp \
    gzipuncompressor.cpp \
    logger.cpp \
    preferences.cpp \
    remotefilefetcher.cpp \
    serialfinder.cpp \
    simpleserialoperation.cpp \
    tararchive.cpp \
    tarziparchive.cpp \
    tarzipuncompressor.cpp \
    tempdirectories.cpp \
    updateregistry.cpp \
    versioninfo.cpp

HEADERS += \
    abstractoperation.h \
    abstractoperationhelper.h \
    abstractoperationrunner.h \
    abstractprotobufmessage.h \
    abstractserialoperation.h \
    applicationbackend.h \
    backenderror.h \
    deviceregistry.h \
    failable.h \
    fileinfo.h \
    filenode.h \
    firmwareupdateregistry.h \
    flipperupdates.h \
    flipperzero/assetmanifest.h \
    flipperzero/filemanager.h \
    flipperzero/pixmaps/default.h \
    flipperzero/pixmaps/updateok.h \
    flipperzero/pixmaps/updating.h \
    flipperzero/protobufsession.h \
    flipperzero/rpc/abstractprotobufoperation.h \
    flipperzero/rpc/guiscreenframeoperation.h \
    flipperzero/rpc/guisendinputoperation.h \
    flipperzero/rpc/guistartscreenstreamoperation.h \
    flipperzero/rpc/guistartvirtualdisplayoperation.h \
    flipperzero/rpc/guistopscreenstreamoperation.h \
    flipperzero/rpc/guistopvirtualdisplayoperation.h \
    flipperzero/rpc/startrpcoperation.h \
    flipperzero/rpc/stoprpcoperation.h \
    flipperzero/rpc/storageinfooperation.h \
    flipperzero/rpc/storagelistoperation.h \
    flipperzero/rpc/storagemkdiroperation.h \
    flipperzero/rpc/storagereadoperation.h \
    flipperzero/rpc/storageremoveoperation.h \
    flipperzero/rpc/storagerenameoperation.h \
    flipperzero/rpc/storagestatoperation.h \
    flipperzero/rpc/storagewriteoperation.h \
    flipperzero/rpc/systemdeviceinfooperation.h \
    flipperzero/rpc/systemfactoryresetoperation.h \
    flipperzero/rpc/systemgetdatetimeoperation.h \
    flipperzero/rpc/systemrebootoperation.h \
    flipperzero/rpc/systemsetdatetimeoperation.h \
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
    flipperzero/radiomanifest.h \
    flipperzero/recovery.h \
    flipperzero/recovery/abstractrecoveryoperation.h \
    flipperzero/recovery/correctoptionbytesoperation.h \
    flipperzero/recovery/exitrecoveryoperation.h \
    flipperzero/recovery/firmwaredownloadoperation.h \
    flipperzero/recovery/setbootmodeoperation.h \
    flipperzero/recovery/wirelessstackdownloadoperation.h \
    flipperzero/recoveryinterface.h \
    flipperzero/rpc/systemupdateoperation.h \
    flipperzero/screenstreamer.h \
    flipperzero/toplevel/abstracttopleveloperation.h \
    flipperzero/toplevel/factoryresetoperation.h \
    flipperzero/toplevel/firmwareinstalloperation.h \
    flipperzero/toplevel/fullrepairoperation.h \
    flipperzero/toplevel/fullupdateoperation.h \
    flipperzero/toplevel/internalupdateoperation.h \
    flipperzero/toplevel/settingsbackupoperation.h \
    flipperzero/toplevel/settingsrestoreoperation.h \
    flipperzero/toplevel/wirelessstackupdateoperation.h \
    flipperzero/utility/abstractutilityoperation.h \
    flipperzero/utility/assetsdownloadoperation.h \
    flipperzero/utility/directorydownloadoperation.h \
    flipperzero/utility/directoryuploadoperation.h \
    flipperzero/utility/factoryresetutiloperation.h \
    flipperzero/utility/getfiletreeoperation.h \
    flipperzero/utility/restartoperation.h \
    flipperzero/utility/startrecoveryoperation.h \
    flipperzero/utility/startupdateroperation.h \
    flipperzero/utility/updateprepareoperation.h \
    flipperzero/utility/userbackupoperation.h \
    flipperzero/utility/userrestoreoperation.h \
    flipperzero/utilityinterface.h \
    flipperzero/virtualdisplay.h \
    gzipuncompressor.h \
    logger.h \
    preferences.h \
    remotefilefetcher.h \
    serialfinder.h \
    simpleserialoperation.h \
    tararchive.h \
    tarziparchive.h \
    tarzipuncompressor.h \
    tempdirectories.h \
    updateregistry.h \
    versioninfo.h

unix|win32 {
    LIBS += -L$$OUT_PWD/../dfu/ -ldfu
}

win32:!win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/dfu.lib

} else:unix|win32-g++ {
    PRE_TARGETDEPS += $$OUT_PWD/../dfu/libdfu.a
}

INCLUDEPATH += $$PWD/../dfu \
               $$PWD/../plugins/protobufinterface

DEPENDPATH += $$PWD/../dfu
