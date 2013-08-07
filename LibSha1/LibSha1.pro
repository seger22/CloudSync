#-------------------------------------------------
#
# Project created by QtCreator 2013-06-26T12:24:41
#
#-------------------------------------------------

QT       -= core gui

TARGET = LibSha1
TEMPLATE = lib

DEFINES += LIBSHA1_LIBRARY

SOURCES += \
    hl_md5.cpp \
    hl_md5wrapper.cpp \
    hl_sha1.cpp \
    hl_sha1wrapper.cpp \
    hl_sha2ext.cpp \
    hl_sha256.cpp \
    hl_sha256wrapper.cpp \
    hl_sha384wrapper.cpp \
    hl_sha512wrapper.cpp \
    hl_wrapperfactory.cpp

HEADERS +=\
        libsha1_global.h \
    hashlibpp.h \
    hl_exception.h \
    hl_hashwrapper.h \
    hl_md5.h \
    hl_md5wrapper.h \
    hl_sha1.h \
    hl_sha1wrapper.h \
    hl_sha2ext.h \
    hl_sha2mac.h \
    hl_sha256.h \
    hl_sha256wrapper.h \
    hl_sha384wrapper.h \
    hl_sha512wrapper.h \
    hl_types.h \
    hl_wrapperfactory.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
