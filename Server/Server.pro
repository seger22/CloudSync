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
Chunker.cpp \
    chunkdat.cpp

HEADERS += \
    session.h \
    server.h \
    Chunker.h \
    chunkdat.h \
    blockChecksumSerial.h \
    chunkhash.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LibHashtable/release/ -lLibHashtable
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LibHashtable/debug/ -lLibHashtable
else:unix: LIBS += -L$$OUT_PWD/../LibHashtable/ -lLibHashtable

INCLUDEPATH += $$PWD/../LibHashtable
DEPENDPATH += $$PWD/../LibHashtable

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DatabaseHandler/release/ -lDatabaseHandler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DatabaseHandler/debug/ -lDatabaseHandler
else:unix: LIBS += -L$$OUT_PWD/../DatabaseHandler/ -lDatabaseHandler

INCLUDEPATH += $$PWD/../DatabaseHandler
DEPENDPATH += $$PWD/../DatabaseHandler

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Chunkhandler/release/ -lChunkhandler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Chunkhandler/debug/ -lChunkhandler
else:unix: LIBS += -L$$OUT_PWD/../Chunkhandler/ -lChunkhandler

INCLUDEPATH += $$PWD/../Chunkhandler
DEPENDPATH += $$PWD/../Chunkhandler



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../FileHandler/release/ -lFileHandler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../FileHandler/debug/ -lFileHandler
else:unix: LIBS += -L$$OUT_PWD/../FileHandler/ -lFileHandler

INCLUDEPATH += $$PWD/../FileHandler
DEPENDPATH += $$PWD/../FileHandler

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DeltaHandler/release/ -lDeltaHandler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DeltaHandler/debug/ -lDeltaHandler
else:unix: LIBS += -L$$OUT_PWD/../DeltaHandler/ -lDeltaHandler

INCLUDEPATH += $$PWD/../DeltaHandler
DEPENDPATH += $$PWD/../DeltaHandler

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Zdelta/release/ -lZdelta
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Zdelta/debug/ -lZdelta
else:unix: LIBS += -L$$OUT_PWD/../Zdelta/ -lZdelta

INCLUDEPATH += $$PWD/../Zdelta
DEPENDPATH += $$PWD/../Zdelta

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/release/ -lboost_thread
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/debug/ -lboost_thread
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/ -lboost_thread

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ConfigurationManager/release/ -lConfigurationManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ConfigurationManager/debug/ -lConfigurationManager
else:unix: LIBS += -L$$OUT_PWD/../ConfigurationManager/ -lConfigurationManager

INCLUDEPATH += $$PWD/../ConfigurationManager
DEPENDPATH += $$PWD/../ConfigurationManager

