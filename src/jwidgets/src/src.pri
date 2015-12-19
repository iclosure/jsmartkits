#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T00:00:11
#
#-------------------------------------------------

################################################################
# creating a precompiled header file (only supported on some platforms (Windows - all MSVC project types,
# Mac OS X - Xcode, Makefile, Unix - gcc 3.3 and up)
################################################################

CONFIG += precompile_header
PRECOMPILED_HEADER = $$PWD/precomp.h
INCLUDEPATH += $$PWD

################################################################
# source fiels
################################################################

################################################
## jwidgets

jwidgets_headers += \
    $$PWD/jwidgets_global.h \
    $$PWD/jwidgets_inc.h \
    $$PWD/jcheckcombobox.h \
    $$PWD/jipaddressedit.h \
    $$PWD/jtreecombobox.h \
    $$PWD/jsplitter.h \
    $$PWD/jspinbox.h \
    $$PWD/jheaderarea.h \
    $$PWD/jtableview.h \
    $$PWD/jtreeview.h \
    $$PWD/jxmltable.h \
    $$PWD/jfiltertableview.h

HEADERS += \
    $$jwidgets_headers \
    $$PWD/private/jspinbox_p.h \
    $$PWD/private/jheaderarea_p.h \
    $$PWD/private/jtableview_p.h \
    $$PWD/private/jtreeview_p.h \
    $$PWD/private/jxmltable_data.h \
    $$PWD/private/jxmltable_widget.h \
    $$PWD/private/jxmltable_p.h \
    $$PWD/private/jfiltertableview_p.h

SOURCES += \
    $$PWD/jwidgets_global.cpp \
    $$PWD/jcheckcombobox.cpp \
    $$PWD/jipaddressedit.cpp \
    $$PWD/jtreecombobox.cpp \
    $$PWD/jsplitter.cpp \
    $$PWD/private/jspinbox_p.cpp \
    $$PWD/jspinbox.cpp \
    $$PWD/private/jheaderarea_p.cpp \
    $$PWD/jheaderarea.cpp \
    $$PWD/private/jtableview_p.cpp \
    $$PWD/jtableview.cpp \
    $$PWD/private/jtreeview_p.cpp \
    $$PWD/jtreeview.cpp \
    $$PWD/private/jxmltable_data.cpp \
    $$PWD/private/jxmltable_widget.cpp \
    $$PWD/private/jxmltable_p.cpp \
    $$PWD/jxmltable.cpp \
    $$PWD/private/jfiltertableview_p.cpp \
    $$PWD/jfiltertableview.cpp

################################################
## resources
################################################

RESOURCES += \
#    resources/jwidgets.qrc

################################################
## framework
################################################

# for jwidgets
framework_headers_jwidgets.version = Versions
framework_headers_jwidgets.path = Headers
framework_headers_jwidgets.files = $$jwidgets_headers

################################################
