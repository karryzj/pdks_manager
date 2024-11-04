DESTDIR = $$OUT_PWD/..

include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)
include($$PWD/../ququan_shapes.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    QuanCavas.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += $$PM_INC $$CM_INC $$PDK_INC
DEPENDPATH += $$PM_INC $$CM_INC $$PDK_INC
LIBS += -L$$DESTDIR -l$$PM_LIB -l$$CM_LIB -l$$PDK_LIB


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
