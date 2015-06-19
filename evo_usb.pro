#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T09:44:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = evo_usb
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hid.c \
    usbhid.cpp \
    bootloader.cpp

HEADERS  += mainwindow.h \
    hidapi.h \
    usbhid.h \
    bootloader.h

FORMS    += mainwindow.ui


win32: LIBS += -L$$PWD/../../Qt/Qt5.4.1/Tools/mingw491_32/i686-w64-mingw32/lib/ -lsetupapi

INCLUDEPATH += $$PWD/../../Qt/Qt5.4.1/Tools/mingw491_32/i686-w64-mingw32/include
DEPENDPATH += $$PWD/../../Qt/Qt5.4.1/Tools/mingw491_32/i686-w64-mingw32/include
