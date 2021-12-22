TEMPLATE = subdirs

SUBDIRS += \
    3rdparty \
    application \
    backend \
    dfu \

backend.depends = dfu 3rdparty
application.depends = dfu 3rdparty backend
