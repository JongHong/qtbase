TEMPLATE      = subdirs
SUBDIRS       = \
                codecs \
                completer \
                customcompleter \
                echoplugin \
                i18n \
                plugandpaint \
                plugandpaintplugins \
                regexp \
                settingseditor \
                styleplugin \
                treemodelcompleter \
                undo \
                undoframework

contains(DEFINES, QT_NO_TRANSLATION): SUBDIRS -= i18n

plugandpaint.depends = plugandpaintplugins
