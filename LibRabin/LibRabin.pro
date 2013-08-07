#-------------------------------------------------
#
# Project created by QtCreator 2013-06-25T19:38:45
#
#-------------------------------------------------

QT       -= core gui

TARGET = LibRabin
TEMPLATE = lib

DEFINES += LIBRABIN_LIBRARY

SOURCES += \
    rabinpoly.cpp \
    msb.cpp

HEADERS +=\
        librabin_global.h \
    rabinpoly.h \
    msb.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
