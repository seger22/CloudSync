#-------------------------------------------------
#
# Project created by QtCreator 2013-08-12T14:13:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = LibHashtable
TEMPLATE = lib

DEFINES += LIBHASHTABLE_LIBRARY


HEADERS +=\
    hash_chain.h \
    hash_chain_prot.h \
    util_class.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
