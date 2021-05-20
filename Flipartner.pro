TEMPLATE = subdirs

SUBDIRS += \
    libDFU \
    libDFUTest

libDFUTest.depends = libDFU
