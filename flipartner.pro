TEMPLATE = subdirs

SUBDIRS += \
    application \
    backend \
    dfu \

backend.depends = dfu
application.depends = dfu backend
