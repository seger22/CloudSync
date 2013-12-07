#-------------------------------------------------
#
# Project created by QtCreator 2013-10-05T01:52:46
#
#-------------------------------------------------

QT       -= gui

TARGET = BlockHandler
TEMPLATE = lib

DEFINES += BLOCKHANDLER_LIBRARY

SOURCES += blockhandler.cpp

HEADERS += blockhandler.h\
        blockhandler_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
