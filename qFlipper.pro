TEMPLATE = subdirs

SUBDIRS += \
    3rdparty \
    application \
    backend \
    dfu \

backend.depends = dfu 3rdparty
application.depends = dfu backend 3rdparty
