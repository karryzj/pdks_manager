CONFIG -= qt
DESTDIR = $$OUT_PWD/..

#DEFINES += MUPARSER_STATIC
DEFINES += MUPARSERLIB_EXPORTS


include($$PWD/../ququan_shapes.pri)
include($$PWD/../lib.pri)
include($$PWD/../ququan_layout.pri)
include($$PWD/../with_all_libs.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    muParser.cpp \
    muParserBase.cpp \
    muParserBytecode.cpp \
    muParserCallback.cpp \
    muParserDLL.cpp \
    muParserError.cpp \
    muParserInt.cpp \
    muParserTokenReader.cpp \
    quanSystem.cpp \
    systemSundialsLink.cpp

HEADERS += \
    muParser.h \
    muParserBase.h \
    muParserBytecode.h \
    muParserCallback.h \
    muParserDLL.h \
    muParserDef.h \
    muParserError.h \
    muParserFixes.h \
    muParserInt.h \
    muParserTemplateMagic.h \
    muParserToken.h \
    muParserTokenReader.h \
    util.h \
    systemSundialsLink.h \
    quanSystem.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

