#-------------------------------------------------
#
# Project created by QtCreator 2013-10-04T20:12:47
#
#-------------------------------------------------



QT       -= gui
QT       += sql

TARGET = DatabaseHandler
TEMPLATE = lib

DEFINES += DATABASEHANDLER_LIBRARY

SOURCES += databasehandler.cpp

HEADERS += databasehandler.h\
Chunker.h\
        databasehandler_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
