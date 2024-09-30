
DESTDIR = $$OUT_PWD/..
TARGET = primitive


include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

DEFINES += MAKE_PRI_LIBRARY

HEADERS += \
    priAttachTreeNodeMgr.h \
    priCircleAction.h \
    priCommon.h \
    priEditNameDialog.h \
    priEllipseAction.h \
    priFiler.h \
    priGraphicsScene.h \
    priGraphicsView.h \
    priImportDialog.h \
    priLayerInfoComboBox.h \
    priPolygenAction.h \
    priQuadrangleAction.h \
    priRectangleAction.h \
    priRotateCfgDialog.h \
    priRulerDialog.h \
    priSectorAction.h \
    priShapeAction.h \
    priShapeActionBase.h \
    priShapeDialog.h \
    priTreeWidgetItem.h \
    priTreeWidgetMenu.h \
    priTriangleAction.h \
    priUtils.h \
    primitiveDefines.h \
    # 锚点UI类
    priAnchorWidget.h \
    priCoordinateWidget.h \
    primitiveMgr.h \
    primitiveMgrWindow.h \
    primitiveWindow.h  \
    priTreeWidget.h \
    primitive.h


SOURCES += \
    priAttachTreeNodeMgr.cpp \
    priCircleAction.cpp \
    priEditNameDialog.cpp \
    priEllipseAction.cpp \
    priFiler.cpp \
    priGraphicsScene.cpp \
    priGraphicsView.cpp \
    # 锚点UI类
    priAnchorWidget.cpp \
    priCoordinateWidget.cpp \
    priImportDialog.cpp \
    priLayerInfoComboBox.cpp \
    priPolygenAction.cpp \
    priRotateCfgDialog.cpp \
    priRulerDialog.cpp \
    priShapeDialog.cpp \
    priTreeWidgetItem.cpp \
    priTreeWidgetMenu.cpp \
    priUtils.cpp \
    primitiveMgr.cpp \
    priQuadrangleAction.cpp \
    priRectangleAction.cpp \
    priSectorAction.cpp \
    priShapeAction.cpp \
    priShapeActionBase.cpp \
    priTriangleAction.cpp \
    primitiveMgrWindow.cpp \
    primitiveWindow.cpp \
    priTreeWidget.cpp \
    primitive.cpp

INCLUDEPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC
DEPENDPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC
LIBS += -L$$DESTDIR -lcommon -lparam -lshape -llayer -lattach_tree

FORMS += \
    priEditNameDialog.ui \
    priRotateCfgDialog.ui
