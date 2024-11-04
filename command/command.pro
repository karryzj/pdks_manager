DESTDIR = $$OUT_PWD/..
TEMPLATE = lib
DEFINES += MAKE_CMD_LIBRARY
TARGET = command

include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

SOURCES += \
    addCommand.cpp \
    commandBase.cpp \
    commandManager.cpp \
    modifyCommand.cpp \
    removeCommand.cpp \
    rotateCommand.cpp

HEADERS += \
    addCommand.h \
    cmdCommon.h \
    cmdDefines.h \
    commandBase.h \
    commandManager.h \
    modifyCommand.h \
    removeCommand.h \
    rotateCommand.h

INCLUDEPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC $$MU_INC $$PR_INC
DEPENDPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC $$MU_INC $$PR_INC
LIBS += -L$$DESTDIR -lcommon -lparam -lshape -llayer -lattach_tree
