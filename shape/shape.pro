
DESTDIR = $$OUT_PWD/..
TARGET = shape

DEFINES += MAKE_SP_LIBRARY

include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)
include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)

HEADERS += \
    shapeBase.h \
    shapeDecl.h \
    shapeDefines.h \
    shapeDrawBooleanProcessor.h \
    shapeFactory.h \
    shapeFactoryBase.h \
    # Shapes that need draw
    shapePolygen.h \
    shapeQuadrangle.h \
    shapeRectangle.h \
    shapeSector.h \
    shapeTriangle.h \
    shapeUtils.h \
    shapeCircle.h \
    shapeEllipse.h \
    # 用于绘制点的Item类
    shapePointGraphicsItem.h\
    # 专门服务于图元绘制的类
    shapeDrawGraphicsItem.h \
    spCommon.h


SOURCES += \
    shapeBase.cpp \
    shapeDecl.cpp \
    shapeDrawBooleanProcessor.cpp \
    shapeFactory.cpp \
    shapeFactoryBase.cpp \
    # Shapes that need draw
    shapePolygen.cpp \
    shapeQuadrangle.cpp \
    shapeRectangle.cpp \
    shapeSector.cpp \
    shapeTriangle.cpp \
    shapeUtils.cpp \
    shapeCircle.cpp \
    shapeEllipse.cpp \
    # 用于绘制点的Item类
    shapePointGraphicsItem.cpp \
    # 专门服务于图元绘制的类
    shapeDrawGraphicsItem.cpp

INCLUDEPATH += $$CM_INC $$PM_INC $$AT_INC $$LY_INC
DEPENDPATH += $$CM_INC $$PM_INC $$AT_INC $$LY_INC
LIBS += -L$$DESTDIR -lcommon -lparam -llayer
