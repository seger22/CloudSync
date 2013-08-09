#-------------------------------------------------
#
# Project created by QtCreator 2013-08-07T10:15:45
#
#-------------------------------------------------

QT       -= core gui

TARGET = LibChecksum
TEMPLATE = lib

DEFINES += LIBCHECKSUM_LIBRARY

SOURCES += \
    checksum.cpp \
    mdfour.cpp \
    rollsum.cpp

HEADERS +=\
        libchecksum_global.h \
    checksum.h \
    librsync.h \
    librsync-config.h \
    mdfour.h \
    rollsum.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
