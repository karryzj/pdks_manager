
DESTDIR = $$OUT_PWD/..
TARGET = attach_tree

DEFINES += MAKE_AT_LIBRARY

include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

HEADERS += \
    AttachTreeNodeData.h \
    atCommon.h \
    attachTreeBaseNode.h \
    attachTreeDefines.h \
    attachTreeNode.h \
    attachTreeNodeMgr.h \
    attachTreePrivateUtils.h \
    attachTreeRootNode.h \
    attachTreeUtils.h

SOURCES += \
    attachTreeBaseNode.cpp \
    attachTreeNode.cpp \
    attachTreeNodeMgr.cpp \
    attachTreePrivateUtils.cpp \
    attachTreeRootNode.cpp \
    attachTreeUtils.cpp

INCLUDEPATH += $$CM_INC $$PM_INC $$SP_INC $$LY_INC
DEPENDPATH += $$CM_INC $$PM_INC $$SP_INC $$LY_INC
LIBS += -L$$DESTDIR -lcommon -lparam -lshape -llayer


