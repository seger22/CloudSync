#-------------------------------------------------
#
# Project created by QtCreator 2013-10-18T23:09:21
#
#-------------------------------------------------

QT+=core
QT       -= gui

TARGET = ConfigurationManager
TEMPLATE = lib

DEFINES += CONFIGURATIONMANAGER_LIBRARY

SOURCES += configurationmanager.cpp \
xml.cpp \
pugixml.cpp

HEADERS += configurationmanager.h\
xml.h \
pugixml.hpp \
        configurationmanager_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
