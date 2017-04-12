#-------------------------------------------------
#
# Project created by QtCreator 2016-04-03T14:48:40
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
    enterwindow.cpp \
    table.cpp \
    lobby.cpp \
    users.cpp \
    cards.cpp \
    game.cpp \
    dialog.cpp

HEADERS  += \
    enterwindow.h \
    table.h \
    lobby.h \
    users.h \
    cards.h \
    game.h \
    dialog.h

FORMS    += \
    enterwindow.ui \
    table.ui \
    lobby.ui \
    dialog.ui

RESOURCES += \
    images.qrc

DISTFILES +=
