#-------------------------------------------------
#
# Project created by QtCreator 2013-10-16T00:38:57
#
#-------------------------------------------------

QT       -= gui

TARGET = Zdelta
TEMPLATE = lib

DEFINES += ZDELTA_LIBRARY

SOURCES +=zdelta.cpp \
deflate.c \
inflate.c \
infblock.c \
zd_mem.c \
inftrees.c \
infcodes.c \
infutil.c \
inffast.c \
adler32.c \
trees.c \
zdu.c \
zutil.c

HEADERS += zdelta.h\
zd_mem.h \
zdconf.h \
zdlib.h \
zutil.h \
inftrees.h \
infblock.h \
infcodes.h \
deflate.h \
infutil.h \
inffast.h \
inffixed.h \
tailor.h \
trees.h \
        zdelta_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
