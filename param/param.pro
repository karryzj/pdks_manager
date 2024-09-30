
DESTDIR = $$OUT_PWD/..
TARGET = param

DEFINES += MAKE_PM_LIBRARY

include($$PWD/../ququan_shapes.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

include($$PWD/../lib.pri)

HEADERS += \
    paramUtils.h \
    paramValidator.h \
    pointE.h \
    expression.h \
    paramDecl.h \
    paramEditDialog.h \
    paramEditWidget.h \
    paramMgr.h \
    paramRuleDialog.h \
    paramRuleWidget.h \
    rule.h \
    pmForceLink.h \
    pmCommon.h \
    transformE.h

SOURCES += \
    paramUtils.cpp \
    paramValidator.cpp \
    pointE.cpp \
    expression.cpp \
    paramDecl.cpp \
    paramEditDialog.cpp \
    paramEditWidget.cpp \
    paramMgr.cpp \
    paramRuleDialog.cpp \
    paramRuleWidget.cpp \
    rule.cpp \
    gsiParamMgr.cpp \
    pmForceLink.cpp \
    transformE.cpp

INCLUDEPATH += $$MU_INC $$TL_INC $$GSI_INC $$CM_INC
DEPENDPATH += $$MU_INC $$TL_INC $$GSI_INC $$CM_INC
LIBS += -L$$DESTDIR -lcommon -lmuParser -lklayout_tl -lklayout_gsi
