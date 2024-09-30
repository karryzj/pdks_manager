DESTDIR = $$OUT_PWD/..
TARGET = pdk_manager


include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

DEFINES += MAKE_PDK_LIBRARY

HEADERS += \
    componentCenterWidget.h \
    drcMgrItemWidget.h \
    drcMgrWidget.h \
    gdsModeWidget.h \
    jsonPaser.h \
    layerSelectItemWidget.h \
    layerSelectWidget.h \
    pdkCommon.h \
    pdkDefines.h \
    pdkInstance.h \
    pdkInstanceMgr.h \
    pdkLayerItemWidget.h \
    pdkLayerMgrWidget.h \
    pdkMgrWindow.h \
    pdkParamTechItemDelegate.h \
    pdkParamTechRuleItemWidget.h \
    pdkParamTechRuleWidget.h \
    priModeWidget.h \
    priSelectDelegate.h \
    primitiveInfoWidget.h \
    techRuleDelegate.h \
    techRuleItemWidget.h \
    techRuleWidget.h

SOURCES += \
    componentCenterWidget.cpp \
    drcMgrItemWidget.cpp \
    drcMgrWidget.cpp \
    gdsModeWidget.cpp \
    jsonPaser.cpp \
    layerSelectItemWidget.cpp \
    layerSelectWidget.cpp \
    pdkInstance.cpp \
    pdkInstanceMgr.cpp \
    pdkLayerItemWidget.cpp \
    pdkLayerMgrWidget.cpp \
    pdkMgrWindow.cpp \
    pdkParamTechItemDelegate.cpp \
    pdkParamTechRuleItemWidget.cpp \
    pdkParamTechRuleWidget.cpp \
    priModeWidget.cpp \
    priSelectDelegate.cpp \
    primitiveInfoWidget.cpp \
    techRuleDelegate.cpp \
    techRuleItemWidget.cpp \
    techRuleWidget.cpp

INCLUDEPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC
DEPENDPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC
LIBS += -L$$DESTDIR -lcommon -lparam -llayer
