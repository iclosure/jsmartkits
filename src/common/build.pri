################################################################
# Data Analyse application
#
# Copyright (C) 2016 Smartsoft
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Smartsoft License, Version 1.0
################################################################

################################################################
# creating a precompiled header file (only supported on some platforms (Windows - all MSVC project types,
# Mac OS X - Xcode, Makefile, Unix - gcc 3.3 and up)
################################################################

CONFIG += precompile_header
PRECOMPILED_HEADER = $$PWD/precomp.h
INCLUDEPATH += $$PWD

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

################################################################
# tmp
################################################################

exists($$JSMARTKITS_ROOT) {
    macx: {
        J_PLAT_PREFIX = macx
    } else:unix: {
        J_PLAT_PREFIX = unix
    }

    J_TEMP_PREFIX = $${JSMARTKITS_ROOT}/temp/source/$$TARGET/$$J_PLAT_PREFIX
    macx:unix: {
        MOC_DIR = $${J_TEMP_PREFIX}
        OBJECTS_DIR = $${J_TEMP_PREFIX}
        RCC_DIR = $${J_TEMP_PREFIX}
    }
}

## extra

exists($${JSMARTKITS_ROOT}/../jdataanalyse) {
    macx:DESTDIR = $${JSMARTKITS_ROOT}/../jdataanalyse/lib
    else:DESTDIR = $${JSMARTKITS_ROOT}/../jdataanalyse/lib/$$jtarget_url
}
