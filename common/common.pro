
DESTDIR = $$OUT_PWD/..
TARGET = common

DEFINES += MAKE_CM_LIBRARY

include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

HEADERS += \
    AES.h \
    common.h \
    common_defines.h \
    crashDump.h \
    crypto.h \
    numUtils.h \
    redoService.h \
    ruler.h \
    scopeTimer.h \
    service.h \
    stringable.h \
    viewport.h

SOURCES += \
    AES.cpp \
    crashDump.cpp \
    crypto.cpp \
    numUtils.cpp \
    redoService.cpp \
    ruler.cpp \
    scopeTimer.cpp \
    service.cpp \
    stringable.cpp \
    viewport.cpp

