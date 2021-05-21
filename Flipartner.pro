TEMPLATE = subdirs

SUBDIRS += \
    FlipartnerGui \
    libDFU \
    #libDFUTest \
    libFlipartner

#libDFUTest.depends = libDFU
FlipartnerGui.depends = libDFU libFlipartner
