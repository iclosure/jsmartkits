#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T00:00:11
#
#-------------------------------------------------

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

JSMARTKITS_ROOT = $$PWD/../../

jwidgets_url = com.smartsoft.jsmartkits.jwidgets

TEMPLATE = lib
win32 {
    TARGET = $$qtLibraryTarget($${jwidgets_url})
} else {
    TARGET = $${jwidgets_url}
}

macx {
    DESTDIR = $${JSMARTKITS_ROOT}/bin
} else {
    DESTDIR = $${JSMARTKITS_ROOT}/bin/jwidgets
}

CONFIG += dll warn_on lib_bundle
#CONFIG += no_keywords silent
#DEFINES += QT_NO_DEBUG_OUTPUT

# - output - tmp -
macx: {
    J_PLAT_PREFIX = macx
} else: unix {
    J_PLAT_PREFIX = unix
}

JWIDGETS_TEMP_PREFIX = $${JSMARTKITS_ROOT}/temp/jwidgets/$${J_PLAT_PREFIX}
macx:unix: {
    MOC_DIR = $${JWIDGETS_TEMP_PREFIX}
    OBJECTS_DIR = $${JWIDGETS_TEMP_PREFIX}
    RCC_DIR = $${JWIDGETS_TEMP_PREFIX}
}

##

contains(CONFIG, dll) {
    DEFINES += JWIDGETS_DLL JWIDGETS_MAKEDLL
}

win32 {
    CONFIG += debug_and_release
    CONFIG += build_all
} else: macx: {
    DEFINES -= Q_CC_CLANG
    CONFIG += qt_framework
} else: unix {
    CONFIG += dll
    LIBS +=
}

include($$PWD/src/src.pri)

macx: CONFIG(qt_framework, qt_framework|qt_no_framework) {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,$$DESTDIR/
    CONFIG += lib_bundle
    QMAKE_BUNDLE_DATA += \
        framework_headers_jwidgets \
        framework_headers_tools_tinyxml
} else {
    copyCommand = @echo ---- module $${jwidgets_url} ----
win32 {
    ## create folder
    destdir = $$DESTDIR/include
    destdir = $$replace(destdir, /, \\)
    ## copy files
    excludefile = $$PWD/copy.ignore
    !exists("$$excludefile"): excludefiles = $$_PRO_FILE_PWD_/copy.ignore
    !exists("$$excludefile"): error("$$excludefile" is not exists!)
    excludefile = $$replace(excludefile, /, \\)
    srcdir = $$PWD/src
    srcdir = $$replace(srcdir, /, \\)
    #exists("$$destdir"): copyCommand += && rd /s /q "$$destdir"
    copyCommand += && xcopy "$$srcdir\\*.h" "$$destdir" /i /s /y /exclude:"$$excludefile"
} else {
    ## create folder
    destdir = $$DESTDIR/include
    ## copy files
    excludefile = $$PWD/copy.ignore
    !exists("$$excludefile"): excludefiles = $$_PRO_FILE_PWD_/copy.ignore
    !exists("$$excludefile"): error("$$excludefile" is not exists!)
    srcdir = $$PWD/src
    !exists("$$destdir"): copyCommand += && mkdir "$$destdir"
    copyCommand += && cp "$$srcdir/*.h" "$$destdir"
    #copyCommand += && xcopy "$$srcdir/*.h" "$$destdir" /i /s /y /exclude:"$$excludefile"
    copyCommand += && rm -f -r "$$destdir/precomp.h"
}

    first.depends = $(first)
    # for jwidgets
    framework_headers_jwidgets.commands = $$copyCommand
    first.depends += framework_headers_jwidgets

    QMAKE_EXTRA_TARGETS += first \
        framework_headers_jwidgets
}
