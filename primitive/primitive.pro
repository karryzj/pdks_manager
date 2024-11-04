DESTDIR = $$OUT_PWD/..
TARGET = primitive


include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

DEFINES += MAKE_PRI_LIBRARY

HEADERS += \
    priAttachTreeNodeMgr.h \
    priAuxiliaryDragBox.h \
    priCircleAction.h \
    priCommon.h \
    priCurveAction.h \
    priCurveInputHBoxLayout.h \
    priConvertShapeDialog.h \
    priDockWidget.h \
    priEditNameDialog.h \
    priEllipseAction.h \
    priFiler.h \
    priGraphicsScene.h \
    priGraphicsView.h \
    priGridResolutionDialog.h \
    priImportDialog.h \
    priLayerInfoComboBox.h \
    priMirrorDialog.h \
    priMouseFollower.h \
    priParamCheckBox.h \
    priPolygenAction.h \
    priQuadrangleAction.h \
    priRectangleAction.h \
    priRotateCfgDialog.h \
    priRounedCornerDialog.h \
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
    primitive.h \
    priPythonWindow.h

SOURCES += \
    priAttachTreeNodeMgr.cpp \
    priAuxiliaryDragBox.cpp \
    priCircleAction.cpp \
    priCurveAction.cpp \
    priCurveInputHBoxLayout.cpp \
    priConvertShapeDialog.cpp \
    priDockWidget.cpp \
    priEditNameDialog.cpp \
    priEllipseAction.cpp \
    priFiler.cpp \
    priGraphicsScene.cpp \
    priGraphicsView.cpp \
    # 锚点UI类
    priAnchorWidget.cpp \
    priCoordinateWidget.cpp \
    priGridResolutionDialog.cpp \
    priImportDialog.cpp \
    priLayerInfoComboBox.cpp \
    priMirrorDialog.cpp \
    priMouseFollower.cpp \
    priParamCheckBox.cpp \
    priPolygenAction.cpp \
    priRotateCfgDialog.cpp \
    priRounedCornerDialog.cpp \
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
    primitive.cpp \
    priPythonWindow.cpp

FORMS += \
    priEditNameDialog.ui \
    priRotateCfgDialog.ui \
    priPythonWindow.ui


INCLUDEPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC $$MU_INC $$CMD_INC
DEPENDPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC $$MU_INC $$CMD_INC
LIBS += -L$$DESTDIR -lcommon -lparam -lshape -llayer -lattach_tree -lcommand
