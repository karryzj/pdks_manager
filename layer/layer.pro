
DESTDIR = $$OUT_PWD/..
TARGET = layer

DEFINES += MAKE_LAYER_LIBRARY

include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)

HEADERS += \
    layInfo.h \
    layListItemWidget.h \
    layStyleDialog.h \
    layStyleFrame.h \
    layWidget.h \
    layerCommon.h \
    layerDefines.h \
    layerMgr.h \
    layerUtils.h

SOURCES += \
    layInfo.cpp \
    layListItemWidget.cpp \
    layStyleDialog.cpp \
    layStyleFrame.cpp \
    layWidget.cpp \
    layerMgr.cpp \
    layerUtils.cpp
