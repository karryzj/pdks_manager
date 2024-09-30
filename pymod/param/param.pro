TARGET = paramcore
REALMODULE = param

include($$PWD/../pymod.pri)

SOURCES = \
  paramMain.cc \

HEADERS += \
  paramMain.h

DESTDIR1 = $$OUT_PWD/../..
LIBS += -L$$DESTDIR1 -lparam

