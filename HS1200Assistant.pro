#-------------------------------------------------
#
# Project created by QtCreator 2013-06-29T21:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HS1200Assistant
TEMPLATE = app

INCLUDEPATH += $$PWD\libusb-win32-bin-1.2.6.0\include

LIBS += -L$$PWD\libusb-win32-bin-1.2.6.0\lib\msvc -llibusb

SOURCES += main.cpp\
        mainwindow.cpp \
    usbworker.cpp \
    acceptscreen.cpp

HEADERS  += mainwindow.h \
    usbworker.h \
    acceptscreen.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

RC_FILE = hs1200icon.rc
