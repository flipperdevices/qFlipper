TEMPLATE = subdirs

SUBDIRS += \
    3rdparty \
    application \
    backend \
    dfu \
    plugins \
    tool

backend.depends = dfu 3rdparty
application.depends = dfu 3rdparty backend
tool.depends = dfu 3rdparty backend
