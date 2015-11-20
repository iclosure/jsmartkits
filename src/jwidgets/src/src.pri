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
# Resouces
################################################################

jwidgets_headers += \
    $$PWD/jwidgets_inc.h \
    $$PWD/jspinbox.h

HEADERS += \
    $$jwidgets_headers \
    $$PWD/jwidgets_global.h

SOURCES += \
    $$PWD/jspinbox.cpp \
    $$PWD/jwidgets_global.cpp

RESOURCES += \
#    resources/jwidget.qrc

##

framework_headers_jwidgets.version = Versions
framework_headers_jwidgets.path = Headers
framework_headers_jwidgets.files = $$jwidgets_headers
