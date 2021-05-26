QT += quick serialport widgets

CONFIG += c++11

SOURCES += \
        main.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    FlipartnerGui_en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

include(../Flipartner_common.pri)

unix: {
    LIBS += \
        -L$$OUT_PWD/../libFlipartner/ -llibFlipartner \
        -L$$OUT_PWD/../libDFU/ -llibDFU

    PRE_TARGETDEPS += \
        $$OUT_PWD/../libFlipartner/liblibFlipartner.a \
        $$OUT_PWD/../libDFU/liblibDFU.a
}

INCLUDEPATH += \
    $$PWD/../libDFU \
    $$PWD/../libFlipartner

DEPENDPATH += \
    $$PWD/../libDFU \
    $$PWD/../libFlipartner

