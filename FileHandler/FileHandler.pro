#-------------------------------------------------
#
# Project created by QtCreator 2013-10-05T01:53:07
#
#-------------------------------------------------

QT       -= gui
QT       += sql

TARGET = FileHandler
TEMPLATE = lib

DEFINES += FILEHANDLER_LIBRARY

SOURCES += filehandler.cpp

HEADERS += filehandler.h\
        filehandler_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/release/ -lboost_filesystem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/debug/ -lboost_filesystem
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/ -lboost_filesystem

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ConfigurationManager/release/ -lConfigurationManager
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ConfigurationManager/debug/ -lConfigurationManager
else:unix: LIBS += -L$$OUT_PWD/../ConfigurationManager/ -lConfigurationManager

INCLUDEPATH += $$PWD/../ConfigurationManager
DEPENDPATH += $$PWD/../ConfigurationManager

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/i386-linux-gnu/release/ -lboost_filesystem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/i386-linux-gnu/debug/ -lboost_filesystem
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/i386-linux-gnu/ -lboost_filesystem

INCLUDEPATH += $$PWD/../../../../../usr/lib/i386-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/i386-linux-gnu

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/i386-linux-gnu/release/ -lboost_system
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/i386-linux-gnu/debug/ -lboost_system
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/i386-linux-gnu/ -lboost_system

INCLUDEPATH += $$PWD/../../../../../usr/lib/i386-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/i386-linux-gnu

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/release/ -lboost_filesystem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/debug/ -lboost_filesystem
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/ -lboost_filesystem

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DatabaseHandler/release/ -lDatabaseHandler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DatabaseHandler/debug/ -lDatabaseHandler
else:unix: LIBS += -L$$OUT_PWD/../DatabaseHandler/ -lDatabaseHandler

INCLUDEPATH += $$PWD/../DatabaseHandler
DEPENDPATH += $$PWD/../DatabaseHandler
