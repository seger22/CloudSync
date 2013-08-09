#-------------------------------------------------
#
# Project created by QtCreator 2013-06-25T19:41:07
#
#-------------------------------------------------

QT       += core
QT       += sql
QT       -= gui

TARGET = Server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    session.cpp \
    server.cpp \
    filehandler.cpp \
    DBmanager.cpp \
    ChunkHandler.cpp \
    Chunker.cpp \
    chunkdat.cpp

HEADERS += \
    session.h \
    server.h \
    filehandler.h \
    DBmanager.h \
    ChunkHandler.h \
    Chunker.h \
    chunkdat.h \
    blockChecksumSerial.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibRabin/release/ -lLibRabin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibRabin/debug/ -lLibRabin
else:unix: LIBS += -L$$OUT_PWD/../LibRabin/ -lLibRabin

INCLUDEPATH += $$PWD/../LibRabin
DEPENDPATH += $$PWD/../LibRabin

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/release/ -lboost_filesystem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/debug/ -lboost_filesystem
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/ -lboost_filesystem

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/release/ -lboost_system
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/debug/ -lboost_system
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/ -lboost_system -pthread

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/release/ -lboost_serialization
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/debug/ -lboost_serialization
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/ -lboost_serialization

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/release/ -lboost_thread
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/debug/ -lboost_thread
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/ -lboost_thread

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibSha1/release/ -lLibSha1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibSha1/debug/ -lLibSha1
else:unix: LIBS += -L$$OUT_PWD/../LibSha1/ -lLibSha1

INCLUDEPATH += $$PWD/../LibSha1
DEPENDPATH += $$PWD/../LibSha1

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibChecksum/release/ -lLibChecksum
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibChecksum/debug/ -lLibChecksum
else:unix: LIBS += -L$$OUT_PWD/../LibChecksum/ -lLibChecksum

INCLUDEPATH += $$PWD/../LibChecksum
DEPENDPATH += $$PWD/../LibChecksum
