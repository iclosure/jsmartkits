#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T00:00:11
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

JSMARTKITS_ROOT = $$PWD/../..

jwidgets_url = com.smartsoft.jsmartkits.jwidgets

TEMPLATE = app
TARGET = $${jwidgets_url}.examples
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

JWIDGETS_TEMP_PREFIX = $${JSMARTKITS_ROOT}/temp/examples/jwidgets/$${J_PLAT_PREFIX}
macx:unix: {
    MOC_DIR = $${JWIDGETS_TEMP_PREFIX}
    OBJECTS_DIR = $${JWIDGETS_TEMP_PREFIX}
    RCC_DIR = $${JWIDGETS_TEMP_PREFIX}
}

## - import libraries -

win32: {
    DEFINES += JWIDGETS_DLL
    CONFIG(debug, debug|release) {
        LIBS += -L$${JSMARTKITS_ROOT}/bin/$$jwidgets_url -l$${jwidgets_url}d
    } else {
        LIBS += -L$${JSMARTKITS_ROOT}/bin/$$jwidgets_url -l$${jwidgets_url}
    }
    INCLUDEPATH += \
        $${JSMARTKITS_ROOT}/src/jwidgets/src
} else: macx: {
    LIBS += \
        -framework $${jwidgets_url} -F$${JSMARTKITS_ROOT}/bin

    INCLUDEPATH += \
        $${JSMARTKITS_ROOT}/bin/$${jwidgets_url}.framework/Versions/Current/Headers

} else: unix {
    LIBS += \
            -L$${JSMARTKITS_ROOT}/bin/jwidgets -l$${jwidgets_url}

    INCLUDEPATH += \
        $${JSMARTKITS_ROOT}/src/jwidgets/src

}

##

HEADERS += \
    test_jxmltable.h \
    test_stylesheet.h

SOURCES += \
    main.cpp \
    test_jxmltable.cpp \
    test_stylesheet.cpp

RESOURCES += \
    resource/jwidgets.qrc

FORMS += \
    test_stylesheet.ui
