TARGET = commoncore
REALMODULE = common

include($$PWD/../pymod.pri)

SOURCES = \
  commonMain.cc \

HEADERS += \
  commonMain.h

DESTDIR1 = $$OUT_PWD/../..
LIBS += -L$$DESTDIR1 -lcommon


