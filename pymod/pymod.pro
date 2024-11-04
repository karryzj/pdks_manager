
DESTDIR = $$OUT_PWD/..

include($$PWD/../ququan_layout.pri)
include($$PWD/../ququan_shapes.pri)

TEMPLATE = subdirs
SUBDIRS = \
  common \
  param \ 


ALL_DIRS = $$SUBDIRS
