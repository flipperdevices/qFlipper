CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib c++11

include(../qflipper_common.pri)

SOURCES += \
    nanopb/pb_common.c \
    nanopb/pb_decode.c \
    nanopb/pb_encode.c

HEADERS += \
    nanopb/pb.h \
    nanopb/pb_common.h \
    nanopb/pb_decode.h \
    nanopb/pb_encode.h
