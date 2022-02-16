TEMPLATE = subdirs

SUBDIRS += \
    3rdparty \
    application \
    backend \
    dfu \
    plugins \
    tool

backend.depends = dfu plugins
application.depends = backend
tool.depends = backend
plugins.depends = 3rdparty
