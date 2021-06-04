TEMPLATE = subdirs

SUBDIRS += \
    Application \
    Backend \
    Dfu \

Backend.depends = Dfu
Application.depends = Dfu Backend
