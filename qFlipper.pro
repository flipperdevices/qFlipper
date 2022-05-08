TEMPLATE = subdirs

SUBDIRS += \
    3rdparty \
    application \
    backend \
    dfu \
    plugins \
    cli \
    tests

backend.depends = dfu plugins
application.depends = backend
cli.depends = backend
plugins.depends = 3rdparty
tests.depends = backend
