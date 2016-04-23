#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T00:00:11
#
#-------------------------------------------------

JSMARTKITS_ROOT = $$PWD/../../..

include($$JSMARTKITS_ROOT/src/common/build.pri)

################################################################
# source fiels
################################################################

################################################
## joffice

joffice_headers += \
    $$PWD/joffice_global.h \
    $$PWD/joffice_inc.h \
    $$PWD/jexcel.h \
    $$PWD/jofficebase.h

HEADERS += \
    $$joffice_headers

SOURCES += \
    $$PWD/joffice_global.cpp \
    $$PWD/jexcel.cpp \
    $$PWD/jofficebase.cpp

################################################
## resources
################################################

RESOURCES += \
    $$PWD/resource/joffice_resource.qrc

################################################
## framework
################################################

# for joffice
framework_headers_joffice.version = Versions
framework_headers_joffice.path = Headers
framework_headers_joffice.files = $$joffice_headers

################################################
