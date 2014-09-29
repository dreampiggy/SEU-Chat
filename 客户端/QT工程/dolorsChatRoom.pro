#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T20:31:59
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dolorsChatRoom
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    tcpsend.cpp \
    tcpreceive.cpp \
    chat.cpp \
    login.cpp \
    logon.cpp


HEADERS  += widget.h \
    tcpsend.h \
    tcpreceive.h \
    chat.h \
    login.h \
    logon.h \
    loginName.h

FORMS    += widget.ui \
    tcpsend.ui \
    tcpreceive.ui \
    chat.ui \
    login.ui \
    logon.ui

RESOURCES += \
    imgs.qrc
