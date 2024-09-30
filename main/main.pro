
DESTDIR = $$OUT_PWD/..
TARGET = ququan_shapes


include($$PWD/../ququan_shapes.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)


SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



INCLUDEPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC $$PR_INC $$PDK_INC
DEPENDPATH += $$CM_INC $$PM_INC $$LY_INC $$AT_INC $$SP_INC $$PR_INC $$PDK_LIB
LIBS += -L$$DESTDIR -lcommon -lparam -llayer -lattach_tree -lshape -lprimitive -lresources -lmuParser -lpdk_manager
