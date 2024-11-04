TARGET = paramcore
REALMODULE = param

include($$PWD/../pymod.pri)

SOURCES = \
  paramMain.cc \

HEADERS += \
  paramMain.h

DESTDIR1 = $$OUT_PWD/../..
LIBS += -L$$DESTDIR1 -lparam


win32 {

  isEmpty(OSTYPE) {
    QMAKE_POST_LINK += && $(COPY_DIR) $$shell_path($$PWD/../distutils_src/ququan) $$shell_path($$DESTDIR_PYMOD/../ququan)
  } else {
    QMAKE_POST_LINK += && $(COPY_DIR) $$PWD/../distutils_src/ququan $$DESTDIR_PYMOD/..
  }

} else {
    QMAKE_POST_LINK += && $(COPY_DIR) $$PWD/../distutils_src/ququan $$DESTDIR_PYMOD/..
}

