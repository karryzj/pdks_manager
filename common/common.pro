
DESTDIR = $$OUT_PWD/..
TARGET = common

DEFINES += MAKE_CM_LIBRARY

include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

HEADERS += \
    AES.h \
    PythonMethod.h \
    common.h \
    commonForceLink.h \
    common_defines.h \
    configManager.h \
    crashDump.h \
    crypto.h \
    numUtils.h \
    ruler.h \
    scopeTimer.h \
    stringable.h \
    viewport.h

SOURCES += \
    AES.cpp \
    PythonMethod.cpp \
    commonForceLink.cpp \
    configManager.cpp \
    crashDump.cpp \
    crypto.cpp \
    gsiPythonMethod.cpp \
    numUtils.cpp \
    ruler.cpp \
    scopeTimer.cpp \
    stringable.cpp \
    viewport.cpp

