#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T00:00:11
#
#-------------------------------------------------


QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

JSMARTKITS_ROOT = $$PWD/../..

joffice_url = com.smartsoft.jsmartkits.joffice

TEMPLATE = app
TARGET = $${joffice_url}.examples
DESTDIR = $${JSMARTKITS_ROOT}/bin

# - output - tmp -
!win32 {
}
# - output - tmp -
macx: {
    J_PLAT_PREFIX = macx
} else: unix {
    J_PLAT_PREFIX = unix
}

JOFFICE_TEMP_PREFIX = $${JSMARTKITS_ROOT}/temp/examples/joffice/$${J_PLAT_PREFIX}
macx:unix: {
    MOC_DIR = $${JOFFICE_TEMP_PREFIX}
    OBJECTS_DIR = $${JOFFICE_TEMP_PREFIX}
    RCC_DIR = $${JOFFICE_TEMP_PREFIX}
}

## - import libraries -

win32: {
    DEFINES += JOFFICE_DLL
    CONFIG(debug, debug|release) {
        LIBS += -L$${JSMARTKITS_ROOT}/bin/$$joffice_url -l$${joffice_url}d
    } else {
        LIBS += -L$${JSMARTKITS_ROOT}/bin/$$joffice_url -l$${joffice_url}
    }
    INCLUDEPATH += \
        $${JSMARTKITS_ROOT}/src/joffice/src
} else: macx: {
    LIBS += \
        -framework $${joffice_url} -F$${JSMARTKITS_ROOT}/bin

    INCLUDEPATH += \
        $${JSMARTKITS_ROOT}/bin/$${joffice_url}.framework/Versions/Current/Headers

} else: unix {
    LIBS += \
            -L$${JSMARTKITS_ROOT}/bin/joffice -l$${joffice_url}

    INCLUDEPATH += \
        $${JSMARTKITS_ROOT}/src/joffice/src

}

##

HEADERS += \
    test_jexcel.h \
    test_jword.h

SOURCES += \
    main.cpp \
    test_jexcel.cpp \
    test_jword.cpp

RESOURCES += \
    resource/joffice.qrc
