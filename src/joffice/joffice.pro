#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T00:00:11
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

JSMARTKITS_ROOT = $$PWD/../..

jtarget_url = com.smartsoft.jsmartkits.joffice

TEMPLATE = lib
win32 {
    TARGET = $$qtLibraryTarget($${jtarget_url})
} else {
    TARGET = $${jtarget_url}
}

macx {
    DESTDIR = $${JSMARTKITS_ROOT}/bin
} else {
    DESTDIR = $${JSMARTKITS_ROOT}/bin/$${jtarget_url}
}

CONFIG += dll warn_on lib_bundle
#CONFIG += no_keywords silent
#DEFINES += QT_NO_DEBUG_OUTPUT

##

contains(CONFIG, dll) {
    DEFINES += JOFFICE_DLL JOFFICE_MAKEDLL
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
        framework_headers_joffice
} else {
    copyCommand = @echo ---- module $${jtarget_url} ----
    win32 {
        ## create folder
        destdir = $$DESTDIR/include
        destdir = $$replace(destdir, /, \\)
        ## copy files
        excludefile = $$PWD/copy.ignore
        !exists("$$excludefile"): excludefile = $$JSMARTKITS_ROOT/src/common/copy.ignore
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
        !exists("$$excludefile"): excludefile = $$JSMARTKITS_ROOT/src/common/copy.ignore
        !exists("$$excludefile"): error("$$excludefile" is not exists!)
        srcdir = $$PWD/src
        !exists("$$destdir"): copyCommand += && mkdir "$$destdir"
        copyCommand += && cp "$$srcdir/*.h" "$$destdir"
        #copyCommand += && xcopy "$$srcdir/*.h" "$$destdir" /i /s /y /exclude:"$$excludefile"
        copyCommand += && rm -f -r "$$destdir/precomp.h"
    }

    first.depends = $(first)
    # for jwidgets
    framework_headers_joffice.commands = $$copyCommand
    first.depends += framework_headers_joffice

    QMAKE_EXTRA_TARGETS += first \
        framework_headers_joffice
}
